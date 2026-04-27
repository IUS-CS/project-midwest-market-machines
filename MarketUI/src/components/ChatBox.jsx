import { useState, useRef, useEffect } from "react";
import "../css/ChatBox.css";

// ChatBox component to display a simple chat interface
// Integrate with a local LLM model
const ChatBox = () => {
  const [isOpen, setIsOpen] = useState(false);
  const [messages, setMessages] = useState([
    { sender: "bot", text: "Welcome to Simple Trade! How can I assist you today?" },
  ]);
  const [input, setInput] = useState("");
  const [isTyping, setIsTyping] = useState(false);
  const messagesEndRef = useRef(null);

  
  useEffect(() => {
    messagesEndRef.current?.scrollIntoView({ behavior: "smooth" });
  }, [messages]);

  // Placeholder function for LLM integration
  // This  will eventually call an API or local function to 
  // get a response from the LLM based on the user's message
  const getLLMResponse = async (userMessage) => {

    
    
    
    await new Promise(resolve => setTimeout(resolve, 1000));
    
    // fake responses until we have the LLM integrated,
    //  can be replaced with actual API response
    const responses = [
      "I'm here to help you with crypto trading insights!",
      "Based on the current market data, what would you like to know?",
      "I can analyze trends and provide trading suggestions.",
      "Feel free to ask me about any cryptocurrency!",
    ];
    
    return responses[Math.floor(Math.random() * responses.length)];
  };

  // Handle sending a message
  const handleSend = async () => {
    if (input.trim() === "") return;

    const userMessage = { sender: "user", text: input };
    
    // Add user message to chat
    setMessages(prev => [...prev, userMessage]);
    setInput("");
    setIsTyping(true);

    try {
      // Get response from LLM
      const botResponse = await getLLMResponse(input);
      
      // Add bot response to chat
      setMessages(prev => [...prev, { sender: "bot", text: botResponse }]);
    } catch (error) {
      console.error("Error getting LLM response:", error);
      setMessages(prev => [...prev, { 
        sender: "bot", 
        text: "Sorry, I encountered an error. Please try again." 
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