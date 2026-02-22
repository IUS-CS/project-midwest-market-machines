# Vision Statement
FOR retail investors WHO seek a “set it and forget it” investment strategy that works for them, THE Simple Trade application is a trading platform THAT automates cryptocurrency and stock trades through the use of local AI models. UNLIKE Composer, which requires extensive configuration by the investor, Simple Trade focuses on simplicity and automaticity. OUR PRODUCT enables retail investors to automate their trading strategy within minutes, not hours.

Retail traders are the target customers for Simple Trade as it will allow them to feel in control and on top of their investments without a large time sink on their behalf. Simple Trade will generate and execute on trading strategies using AI tooling.

# Proposal
Simple Trade is an AI-assisted trading application for major cryptocurrencies and stocks. This application runs entirely on-device. In doing so, both the developers of Simple Trade and its users are able to eliminate costs from cloud hosting and inference; enhance the privacy and security of the Simple Trade application by ensuring no user data is every processed or stored by the developers of Simple Trade, and more fully utilize the hardware the user has already paid for.
It is our goal that Simple Trade will truly enable retail traders to maximize their return on investment without becoming a time sink, or worse, a Tool Shaped Object. Rather, Simple Trade will provide immediate value and ease-of-use to retail traders. We strive not to create an application where the product of the application is the experience of setting up and using the application, but rather one that is genuinely useful to its users. 

## Technology
Simple Trade, in its simplest form, will be little more than a live ticker for the price of Bitcoin. While the team works to gain a footing, we have intentionally limited the scope down to a very simple set of core features. Additionally, we have chosen to write the Simple Trade prototype for a Windows 10/11 desktop environment, as that is what each member of the team has access to.

We will be using:
-React Native for cross-platform and mobile development (with Windows and macOS out-of-tree support),
  -We foresee a problem where this may be too complicated, and we will have to either move to a web app via Electron, or pick mobile vs. desktop.
  -We will cross this bridge when we come to it.
-C++ for the backend,
-JavaScript, HTML, and CSS for the frontend.
-We will maintain CMake as the build system for the C++ code during the prototyping phase
-Same for Node.js for the JavaScript front end.
-Both to be replaced by React Native’s build system as necessary.
Initially, we will set up our application to:
-Be a client-server setup between C++ and JavaScript.
-We will use WebSockets to communicate between the two processes.
  -Lowest effort way to get this to work before switching to React.
-The backend C++ code will connect to Binance’s WebSocket API.
-The backend C++ code will communicate to and work with any ML model used.

