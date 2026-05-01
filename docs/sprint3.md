# Sprint Ceremony Minutes
  
Date: 2026-04-20

Members present:

* Ian Olsen
* Xavier Olsen
* Tanner Philips
* Hunter Poole

## Demo

This sprint, we completed:

* Integrated shadcn/ui into the frontend
* Added a chatbox for a future LLM call
* Created a table for the user's holdings.
* Created a CSV database to hold user's trades.

### Good

* The items we created were significant in "weight." As in, we have completed or nearly completed a few key features.
* Ian and Xavier worked on two parts of the same feature, and found that parallel workflow to be quite good.
* The shadcn/ui components really helped speed up frontend work.
* Despite Hunter having to leave without finishing his feature(s), they were fleshed out enough for others to work with them.

### Bad

* Ian and Xavier worked on the same feature on separate branches - which makes the PRs a bit messy to review.
* Hunter had to leave to catch a flight before I could the feature(s) I was working on, as in, bad time management.

### Actionable Commitments

* Hunter will start on his work earlier, such that he can finish in time for a PR review and merge to main.
* We will use draft PRs to make our code changes visible to others, even if they are not ready yet.

## Next Sprint Planning

* We will integrate an API call out to one of the Gemma models from Google, or perhaps a Gemini Lite model depending on cost and response quality.
* Hunter will commit to finishing the table and adding the buy/sell buttons.
* Xavier will finish sending the historical data to the frontend.
* We will find a way to keep the charts alive when they are not rendered.
* We will finish testing.

Stretch:

* We will try to get automated trading going as a stretch goal, but do not consider this as important as the previously stated goals.
* We will try to get a summary of the user's current holdings (as in, their cumulative value at least).

Points | Story
-------|--------
3      | As a user, I want to view historical data, so that I can make trades based on historical performance.
5      | As a user, I want to ask an LLM about which coins I should buy, so I can be more informed about what trades to make.
2      | As a user, I want to be able to paper trade to try out different trading strategies.
11     | As a user, I want to be able to automate my trading, such that I stay in a good position with minimal effort
