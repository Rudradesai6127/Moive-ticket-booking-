# Test Cases — Movie Ticket Booking System

Run each test on a clean copy of `movies.txt` and `bookings.txt` where noted. Record actual output and pass/fail after execution.

| No. | Test / Input | Expected result |
|---:|---|---|
| 1 | Start with no `movies.txt` file | Three sample movies are created and saved |
| 2 | Choose View Movies | Movie ID, name, price, and seats are displayed |
| 3 | Admin adds a movie with valid name, price, seats | Movie is added and saved |
| 4 | Search for an existing movie ID | Matching movie is displayed |
| 5 | Search for a non-existing movie ID | “Movie not found” message |
| 6 | Update the price of an existing movie | New price is shown and persisted |
| 7 | Update the seat count to a non-negative value | Seat count is updated and persisted |
| 8 | Delete a movie with no confirmed bookings | Movie is removed |
| 9 | Attempt to delete a movie with a confirmed booking | Deletion is blocked |
| 10 | Book 2 tickets when at least 2 seats remain | Booking succeeds, total is price × 2, seats decrease by 2 |
| 11 | Enter 0 or a negative ticket count | Input is rejected |
| 12 | Request more tickets than available | Booking is rejected and seats do not change |
| 13 | Search for an existing booking ID | Booking details are displayed |
| 14 | Update a booking to a valid ticket count | Total and remaining seats are adjusted |
| 15 | Cancel a confirmed booking, exit, and restart | Booking status remains CANCELLED and seats are restored |

## Result recording
For each test, add:
- Actual result:
- Pass/Fail:
- Date:
- Tester:
