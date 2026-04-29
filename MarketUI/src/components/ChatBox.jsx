import { useState, useRef, useEffect } from "react";
import "../css/ChatBox.css";

/* ChatBox.jsx
 *
 * This component renders a floating AI chat assistant powered by a local
 * Ollama instance. It exposes two behaviors:
 *    1. Manual chat - the user types a message and the model responds
 *       with awareness of the current coin, price, holdings, and
 *       transaction history passed in via marketContext.
 *    2. Auto-scan - a 30 second interval fires automatically, passing
 *       all live coin prices to the model and asking it to pick one
 *       to buy. The result is executed via onTrade() and logged in chat.
 *
 */

const OLLAMA_URL = "http://localhost:11434/api/chat";
const COINS = ["BTCUSDT", "ETHUSDT", "ADAUSDT", "XRPUSDT", "DOTUSDT", "UNIUSDT"];

/* buildSystemPrompt()
 *
 * Returns the system prompt string injected on every request to Ollama.
 * Instructs the model to respond ONLY with a raw JSON object containing
 * four fields: message, action, quantity, and coin.
 * Including coin in the response allows the AI to target any symbol,
 * not just the one currently selected in the Watchlist.
 */
const buildSystemPrompt = () => `You are a local AI trading assistant for the Crypto Trading App "Simple Trade".
You have access to live market data, current holdings, and transaction history.

CRITICAL: Respond ONLY with a raw JSON object. No markdown, no backticks, no intro text.

Format:
{
  "message": "your concise response (max 2 sentences)",
  "action": "buy" | "sell" | "none",
  "quantity": number,
  "coin": "BTCUSDT" | "ETHUSDT" | "ADAUSDT" | "XRPUSDT" | "DOTUSDT" | "UNIUSDT"
}

Rules:
- If the user asks what they own, summarize holdings clearly.
- If the user asks about trade history, reference recent transactions.
- Never invent data not present in the context block.
- Always remind the user this is a paper trading simulation.`;

/* buildContextString()
 *
 * Assembles the market context block injected into every manual chat request.
 * Sections:
 *    - MARKET CONTEXT: selected coin and its live price.
 *    - ALL COIN PRICES: flattened allCoinsSnapshot so the LLM can reference
 *      any coin's current price when the user asks about it.
 *    - CURRENT HOLDINGS: what the user currently holds.
 *    - RECENT TRANSACTIONS: last 10 trades for personalized responses.
 */
const buildContextString = (marketContext) => {
  const { price, coin, holdings, transactions, allCoinsSnapshot } = marketContext;

  const allPrices = allCoinsSnapshot
    ? Object.entries(allCoinsSnapshot).map(([c, p]) => `${c}: $${p}`).join(" | ")
    : "Loading...";
  
  const holdingsSummary = holdings?.length
    ? holdings.map((h) => `${h.coin}: ${h.quantity}`).join(", ")
    : "No current holdings";
  
  const transactionSummary = transactions?.length
    ? transactions
        .slice(-10)
        .map((t) => `${t.type?.toUpperCase() ?? "TRADE"} ${t.quantity} ${t.coin} @ $${t.price}`)
        .join(" | ")
    : "No transaction history";
  return `
=== MARKET CONTEXT ===
SELECTED COIN: ${coin ?? "N/A"} | LIVE PRICE: $${price ?? "N/A"}

=== ALL COIN PRICES ===
${allPrices}

=== CURRENT HOLDINGS ===
${holdingsSummary}

=== RECENT TRANSACTIONS (last 10) ===
${transactionSummary}
  `.trim();
};

/* buildAutoScanPrompt()
 *
 * Builds the prompt used during the 30 second auto scan interval,
 * Passes all current coin prices and asks Qwen to pick one to buy,
 * Gave technical analysis responsibility to Qwen through sysprompt,
 * which prevents it from overwhelmingly prefering no action.
 * Temperature is set to 0.8 at call time to encourage varied picks across successive scans.
 */
const buildAutoScanPrompt = (allCoinsSnapshot) => {
  const prices = allCoinsSnapshot
    ? Object.entries(allCoinsSnapshot).map(([c, p]) => `${c}: $${p}`).join("\n")
    : "No data yet";

  return `
=== AUTO SCAN: ALL COIN PRICES ===
${prices}

You are professional technical analyst doing a routine market scan, Pick ONE coin from the list above to buy or sell 1 unit of,
or choose to take no action. Base your decision purely on the price data provided, you have no other information. 
Respond ONLY with a JSON object containing action, quantity, and coin fields. Action can be "buy", "sell", or "none". 
Quantity should be 1 if buying or selling, and 0 if taking no action. Coin should be the symbol of the coin you choose to trade, 
or blank if action is "none".
  `.trim();
};

// ChatBox component — renders the floating chat window and manages message state
const ChatBox = ({ marketContext = {}, onTrade }) => {
  const [isOpen, setIsOpen] = useState(false);
  const [messages, setMessages] = useState([
    { sender: "bot", text: "Welcome to Simple Trade! Automated trading is active, how else can I assist you today?" },
  ]);
  const [input, setInput] = useState("");
  const [isTyping, setIsTyping] = useState(false);
  const messagesEndRef = useRef(null);

  useEffect(() => {
    messagesEndRef.current?.scrollIntoView({ behavior: "smooth" });
  }, [messages]);

  /* Auto-scan useEffect
   * Fires every 30 seconds independently, passes all live coin prices 
   * to the model via buildAutoScanPrompt() and asks it to pick one
   * coin to buy. Executes the trade via onTrade() and appends an [AUTO-SCAN]
   * message to the chat log so the user can see what and why the LLM chose what
   * it did,
   * Silently skips if Ollama is unreachable or no price data yet, to avoid spam.
   */
  useEffect(() => {
    const interval = setInterval(async () => {
      const { allCoinsSnapshot } = marketContext;
      if (!allCoinsSnapshot || Object.keys(allCoinsSnapshot).length < COINS.length) return;

      try {
        const payload = {
          model: "qwen2.5:1.5b",
          messages: [
            { role: "system", content: buildSystemPrompt() },
            { role: "user", content: buildAutoScanPrompt(allCoinsSnapshot) },
          ],
          stream: false,
          options: { temperature: 0.8 },
          format: "json",
        };

        const response = await fetch(OLLAMA_URL, {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify(payload),
        });

        if (!response.ok) return;

        const data = await response.json();
        const parsed = JSON.parse(data.message?.content ?? "{}");

        const action = ["buy", "sell"].includes(parsed.action) ? parsed.action : null;
        const quantity = Number(parsed.quantity) || 1;
        const coin = COINS.includes(parsed.coin) ? parsed.coin : null;
        const message = parsed.message || "Market scan complete.";

        if (action !== null && coin !== null && onTrade) {
            onTrade(action, quantity, coin);
            setMessages((prev) => [
            ...prev,
          { sender: "bot", text: `[AUTO-SCAN] ${message}\n[EXECUTED]: ${action.toUpperCase()} ${quantity} ${coin}` },
         ]);
         } else {  
          setMessages((prev) => [
          ...prev,
          { sender: "bot", text: `[AUTO-SCAN] ${message}` },
          ]);
          }
      } catch {
        // silent skip - don't flood the chat with connection errors during auto-scan
      }
    }, 30000);

    return () => clearInterval(interval);
  }, [marketContext, onTrade]);

  /* getLLMResponse()
   *
   * Sends the user's message along with the full market context block
   * to the local Ollama instance at OLLAMA_URL, parses the returned JSON
   * and extracts 
   * message, 
   * action, 
   * quantity, and 
   * coin.
   * Safeguards against malformed JSONs because Qwen can be really stupid and I don't 
   * trust it
   * 
   * Fall back json flag returns the raw text as the message instead.
   */
  const getLLMResponse = async (userMessage) => {
    const contextBlock = buildContextString(marketContext);

    const payload = {
      model: "qwen2.5:1.5b",
      messages: [
        { role: "system", content: buildSystemPrompt() },
        { role: "user", content: `${contextBlock}\n\nUser: ${userMessage}` },
      ],
      stream: false,
      options: { temperature: 0.1 },
      format: "json",
    };

    const response = await fetch(OLLAMA_URL, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(payload),
    });

    if (!response.ok) throw new Error(`Ollama Offline (Status ${response.status})`);

    const data = await response.json();
    const rawText = data.message?.content ?? "{}";

    try {
      const parsed = JSON.parse(rawText);
      return {
        message: parsed.message || "Request processed.",
        action: ["buy", "sell"].includes(parsed.action) ? parsed.action : "none",
        quantity: Number(parsed.quantity) || 0,
        coin: parsed.coin ?? marketContext.coin,
      };
    } catch {
      return { message: rawText, action: "none", quantity: 0, coin: marketContext.coin };
    }
  };

  /* handleSend()
   *
   * Reads the current textarea input, appends the user message to the
   * chat log, then calls getLLMResponse() to get the model's reply.
   * If the model returns a valid trade action with a positive quantity,
   * it calls onTrade() with the action, quantity, and coin, then logs
   * an [EXECUTED] confirmation. Otherwise just appends the reply text.
   */
  const handleSend = async () => {
    if (input.trim() === "") return;

    const userMessage = { sender: "user", text: input };

    // Add user message to chat
    setMessages(prev => [...prev, userMessage]);
    setInput("");
    setIsTyping(true);

    try {
      // Get response from LLM
      const { message, action, quantity, coin } = await getLLMResponse(input);

      if (action !== "none" && quantity > 0 && onTrade) {
        onTrade(action, quantity, coin);
        setMessages(prev => [...prev, {
          sender: "bot",
          text: `${message}\n\n[EXECUTED]: ${action.toUpperCase()} ${quantity} ${coin}`,
        }]);
      } else {
        // Add bot response to chat
        setMessages(prev => [...prev, { sender: "bot", text: message }]);
      }
    } catch (error) {
      console.error("Error getting LLM response:", error);
      setMessages(prev => [...prev, {
        sender: "bot",
        text: "Sorry, I encountered an error. Please try again.",
      }]);
    } finally {
      setIsTyping(false);
    }
  };

  // Handle Enter key press
  const handleKeyPress = (e) => {
    if (e.key === "Enter" && !e.shiftKey) {
      e.preventDefault();
      handleSend();
    }
  };

  return (
    <>
      {/* Floating toggle button */}
      <button
        className={`chatbox-toggle ${isOpen ? 'open' : ''}`}
        onClick={() => setIsOpen(!isOpen)}
        aria-label="Toggle chat"
      >
        {isOpen ? '✕' : '?'}
      </button>

      {/* Chat window */}
      {isOpen && (
        <div className="chatbox-container">
          <div className="chatbox-header">
            <h3>AI Trading Assistant</h3>
            <button
              className="chatbox-close"
              onClick={() => setIsOpen(false)}
              aria-label="Close chat"
            >
              ✕
            </button>
          </div>

          <div className="chatbox-messages">
            {messages.map((msg, index) => (
              <div key={index} className={`message ${msg.sender}`}>
                <div className="message-bubble">
                  <p>{msg.text}</p>
                </div>
              </div>
            ))}

            {isTyping && (
              <div className="message bot">
                <div className="message-bubble typing">
                  <span className="typing-dot"></span>
                  <span className="typing-dot"></span>
                  <span className="typing-dot"></span>
                </div>
              </div>
            )}

            <div ref={messagesEndRef} />
          </div>

          <div className="chatbox-input">
            <textarea
              value={input}
              onChange={(e) => setInput(e.target.value)}
              onKeyPress={handleKeyPress}
              placeholder="Ask me anything about crypto..."
              rows="1"
            />
            <button onClick={handleSend} disabled={input.trim() === ""}>
              Send
            </button>
          </div>
        </div>
      )}
    </>
  );
};

export default ChatBox;
