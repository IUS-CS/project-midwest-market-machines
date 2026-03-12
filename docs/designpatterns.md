## 1. Introduction

In our current project, the closest analogue to an OODesign pattern is the React component App.jsx as the Subject of the Observer Design pattern, with the WatchList.jsx, ShowPrice.jsx, and CandlestickChart.jsx behaving as Observers. 

While App.jsx currently lacks an explicit interface, the application state currentCoin  acts as the Subject by triggering a re-render, updating which dependent components display in the app.  

This ensures the Frontend UX remains in sync with the websocket datastream, in line with the Intent section of the OODesign.com/observer-pattern as it “Defines a one-to-many dependency between objects so that when one object changes state, all its dependents are notified and updated automatically.” 

 

## 2. Potential Patterns

* A Singleton class could be used to save user configurations and reload them on launch. 

* A Null instance would be useful in the Searchbar component, in the case that a coin matching the search is not found, a null option could be used with default functionality. 

* The Command design is a strong fit for making generic buying lifecycle commands for the user with manual input and A.I. to interact with the app in a unified way, and which will likely need the Interpreter. 

* The Interpreter would be necessary to wicks, highs, lows, technical analysis tokenization for A.I. access
## 3. Future Design Choices

We have been considering putting charts in the background to keep the websocket streams contiguous, but using Memento design patterns it may be possible to build a 5 minute snapshot of only the necessary .json data to rebuild the graph, allowing for them to be rebuilt using a rolling 5 minute window until the user selects the graph, rather than keeping them open in the background.  
In the current build, the websocket sends all data to the App.jsx, which then relays to the other components based on the currentCoin’s state. It may be beneficial to use the Chain of Responsibility strategy to route information through a handler, allowing the websocket routing logic to be decoupled from App.jsx’s state. 

If in the future to we implement the option to choose the chart type, an Adapter class may be useful for formatting the requests between different graph types, allowing information for building the chart to be stored in a separate class from its constructor. However, because of the fundamental differences in the data required to build graphs, this would also need to change which websocket stream is active. 

In an ongoing branch by Hunter, repeatable logic in our main backend file has been split into a pair of interfaces ExchangeClient.h and BinanceProcessor.h. This split facilitates the creation of specific tests as well as objects of the classes. This decoupling is a step toward the Strategy design pattern and the potential for other data sources, such as coinbase, to be used in the future.

Currently, a primitive Iterator builds the list of active Watchlist coins, and a redundant process builds its corresponding graph. Combining the WatchList and CandlestickChart component and using a more complex Iterator would make the construction of the set of active charts more streamlined. 

During Sprint 1, we opened a collaborative Figma account, by moving our prototyping and systems design there, the information can be better communicated, with visual, logical, and textual components like OOdesign instead of just text abstractions.
