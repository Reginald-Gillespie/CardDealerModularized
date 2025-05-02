## CardDealerModularized

This code aims to modularize and abstract the code for the Hack Pack card dealer, making it extremely easy to drop in new games.

I waded through these 4K lines of code to abstract the Card Dealer's games into a games folder, so you can **easily create your own game**. 

Here is a small list of features this code adds:
- `dispenceCards(amount);` spit out a single card, or rapidfire multiple cards.
- `nextTurn(skipAmount);` allows you to skip playsers.
- Don't quit the game when a card couldn't dispense. 
- Easily set how many cards to deal to each person.
- Buttons pin numbers are stored in an `enum` so you don't need to memorize which button was which pin.
- Easily manage scrolling messages while waiting for player.
- Change card dealer faces.
- More readable code
- And more

---

To resolve issues or hang out, you can join the official Hack Pack Discord [here](https://mee6.xyz/i/RAMmVgdtYZ).