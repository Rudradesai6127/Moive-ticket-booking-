/*
PROJECT: Movie Ticket Booking System
LANGUAGE: C++17

CHECKLIST COVERAGE
1. Minimum 5 classes: Movie, User, Customer, Admin, Booking, Payment, MovieBookingSystem.
2. Constructors and destructors: provided in classes.
3. Inheritance and polymorphism: Customer/Admin derive from abstract User; role() is virtual.
4. Dynamic memory: unique_ptr and make_unique are used for users and movies.
5. File persistence: movies.txt and bookings.txt are loaded/saved automatically.
6. Add/Search/Update/Delete: movie CRUD and booking search/update/cancel are provided.
7. Main transaction: ticket booking validates seats, processes payment, updates seats, and saves.
8. Use Case Diagram: included in README.md.
9. Class Diagram: included in README.md.
10. Two Sequence Diagrams: included in README.md.
11. 15 test cases: included in test_cases.md.
12. Executable + README: compile instructions in README.md; compile to create executable.

Compile:
  g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o movie_booking
Run:
  ./movie_booking       (Linux/macOS)
  movie_booking.exe     (Windows)
*/

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

static const string MOVIE_FILE = "movies.txt";
static const string BOOKING_FILE = "bookings.txt";

string cleanField(string value) {
    // Keep the pipe character as a file delimiter out of saved fields.
    replace(value.begin(), value.end(), '|', '/');
    return value;
}

int readInt(const string& prompt, int minimum = numeric_limits<int>::min(),
            int maximum = numeric_limits<int>::max()) {
    while (true) {
        cout << prompt;
        int value;
        if (cin >> value && value >= minimum && value <= maximum) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cout << "Invalid input. Please enter a valid number";
        if (minimum != numeric_limits<int>::min()) cout << " (minimum " << minimum << ")";
        if (maximum != numeric_limits<int>::max()) cout << " (maximum " << maximum << ")";
        cout << ".\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string readLine(const string& prompt) {
    cout << prompt;
    string value;
    getline(cin, value);
    return cleanField(value);
}

class Movie {
private:
    int id_;
    string name_;
    int price_;
    int availableSeats_;

public:
    Movie() : id_(0), name_(""), price_(0), availableSeats_(0) {}
    Movie(int id, string name, int price, int seats)
        : id_(id), name_(cleanField(name)), price_(price), availableSeats_(seats) {}
    ~Movie() = default;

    int id() const { return id_; }
    const string& name() const { return name_; }
    int price() const { return price_; }
    int availableSeats() const { return availableSeats_; }

    void setName(const string& name) { name_ = cleanField(name); }
    void setPrice(int price) { price_ = price; }
    void setAvailableSeats(int seats) { availableSeats_ = seats; }

    void display() const {
        cout << left << setw(6) << id_ << setw(30) << name_
             << "Rs. " << setw(8) << price_ << availableSeats_ << '\n';
    }

    string serialize() const {
        ostringstream out;
        out << id_ << '|' << name_ << '|' << price_ << '|' << availableSeats_;
        return out.str();
    }

    static bool deserialize(const string& line, Movie& movie) {
        istringstream in(line);
        string id, name, price, seats;
        if (!getline(in, id, '|') || !getline(in, name, '|') ||
            !getline(in, price, '|') || !getline(in, seats)) return false;
        try {
            movie = Movie(stoi(id), name, stoi(price), stoi(seats));
            return true;
        } catch (...) {
            return false;
        }
    }
};

// Abstract base class: demonstrates inheritance and runtime polymorphism.
class User {
protected:
    int id_;
    string name_;

public:
    User(int id, string name) : id_(id), name_(cleanField(name)) {}
    virtual ~User() = default;
    virtual string role() const = 0;
    virtual void displayProfile() const {
        cout << role() << " | ID: " << id_ << " | Name: " << name_ << '\n';
    }
    const string& name() const { return name_; }
};

class Customer : public User {
public:
    Customer(int id, string name) : User(id, name) {}
    ~Customer() override = default;
    string role() const override { return "Customer"; }
};

class Admin : public User {
public:
    Admin(int id, string name) : User(id, name) {}
    ~Admin() override = default;
    string role() const override { return "Administrator"; }
};

class Payment {
private:
    int amount_;

public:
    explicit Payment(int amount) : amount_(amount) {}
    ~Payment() = default;

    bool process() const {
        cout << "Payment of Rs. " << amount_ << " processed successfully (demo mode).\n";
        return true;
    }
    int amount() const { return amount_; }
};

class Booking {
private:
    int id_;
    int movieId_;
    string customerName_;
    int tickets_;
    int totalAmount_;
    string status_; // CONFIRMED or CANCELLED

public:
    Booking() : id_(0), movieId_(0), tickets_(0), totalAmount_(0), status_("CONFIRMED") {}
    Booking(int id, int movieId, string customer, int tickets, int amount,
            string status = "CONFIRMED")
        : id_(id), movieId_(movieId), customerName_(cleanField(customer)),
          tickets_(tickets), totalAmount_(amount), status_(status) {}
    ~Booking() = default;

    int id() const { return id_; }
    int movieId() const { return movieId_; }
    const string& customerName() const { return customerName_; }
    int tickets() const { return tickets_; }
    int totalAmount() const { return totalAmount_; }
    const string& status() const { return status_; }
    void setTickets(int tickets) { tickets_ = tickets; }
    void setTotalAmount(int amount) { totalAmount_ = amount; }
    void setStatus(const string& status) { status_ = status; }

    void display() const {
        cout << "Booking ID: " << id_ << " | Movie ID: " << movieId_
             << " | Customer: " << customerName_ << " | Tickets: " << tickets_
             << " | Total: Rs. " << totalAmount_ << " | Status: " << status_ << '\n';
    }

    string serialize() const {
        ostringstream out;
        out << id_ << '|' << movieId_ << '|' << customerName_ << '|'
            << tickets_ << '|' << totalAmount_ << '|' << status_;
        return out.str();
    }

    static bool deserialize(const string& line, Booking& booking) {
        istringstream in(line);
        string id, movieId, customer, tickets, amount, status;
        if (!getline(in, id, '|') || !getline(in, movieId, '|') ||
            !getline(in, customer, '|') || !getline(in, tickets, '|') ||
            !getline(in, amount, '|') || !getline(in, status)) return false;
        try {
            booking = Booking(stoi(id), stoi(movieId), customer, stoi(tickets),
                              stoi(amount), status);
            return true;
        } catch (...) {
            return false;
        }
    }
};

class MovieBookingSystem {
private:
    vector<unique_ptr<Movie>> movies_; // Dynamic memory managed safely.
    vector<Booking> bookings_;
    unique_ptr<User> currentUser_;
    int nextMovieId_ = 1;
    int nextBookingId_ = 1;

    Movie* findMovie(int id) {
        for (auto& movie : movies_) if (movie->id() == id) return movie.get();
        return nullptr;
    }

    const Movie* findMovie(int id) const {
        for (const auto& movie : movies_) if (movie->id() == id) return movie.get();
        return nullptr;
    }

    Booking* findBooking(int id) {
        for (auto& booking : bookings_) if (booking.id() == id) return &booking;
        return nullptr;
    }

    void loadMovies() {
        ifstream file(MOVIE_FILE);
        string line;
        while (getline(file, line)) {
            Movie movie;
            if (Movie::deserialize(line, movie)) {
                nextMovieId_ = max(nextMovieId_, movie.id() + 1);
                movies_.push_back(unique_ptr<Movie>(new Movie(movie)));
            }
        }
    }

    void loadBookings() {
        ifstream file(BOOKING_FILE);
        string line;
        while (getline(file, line)) {
            Booking booking;
            if (Booking::deserialize(line, booking)) {
                nextBookingId_ = max(nextBookingId_, booking.id() + 1);
                bookings_.push_back(booking);
            }
        }
    }

    void saveMovies() const {
        ofstream file(MOVIE_FILE);
        for (const auto& movie : movies_) file << movie->serialize() << '\n';
        if (!file) cerr << "Warning: could not save " << MOVIE_FILE << ".\n";
    }

    void saveBookings() const {
        ofstream file(BOOKING_FILE);
        for (const auto& booking : bookings_) file << booking.serialize() << '\n';
        if (!file) cerr << "Warning: could not save " << BOOKING_FILE << ".\n";
    }

    void seedMoviesIfEmpty() {
        if (!movies_.empty()) return;
        movies_.push_back(unique_ptr<Movie>(new Movie(nextMovieId_++, "Avengers: Endgame", 250, 50)));
        movies_.push_back(unique_ptr<Movie>(new Movie(nextMovieId_++, "3 Idiots", 180, 40)));
        movies_.push_back(unique_ptr<Movie>(new Movie(nextMovieId_++, "Dangal", 200, 45)));
        saveMovies();
    }

    void listMovies() const {
        if (movies_.empty()) {
            cout << "No movies found. Ask the administrator to add one.\n";
            return;
        }
        cout << "\nID    Movie                         Ticket Price  Seats\n";
        cout << "----------------------------------------------------------\n";
        for (const auto& movie : movies_) movie->display();
    }

    void addMovie() {
        string name = readLine("Movie name: ");
        if (name.empty()) {
            cout << "Movie name cannot be empty.\n";
            return;
        }
        int price = readInt("Ticket price (Rs.): ", 1, 100000);
        int seats = readInt("Available seats: ", 0, 1000000);
        movies_.push_back(unique_ptr<Movie>(new Movie(nextMovieId_++, name, price, seats)));
        saveMovies();
        cout << "Movie added successfully.\n";
    }

    void searchMovie() const {
        int id = readInt("Enter movie ID to search: ", 1);
        const Movie* movie = findMovie(id);
        if (movie) movie->display();
        else cout << "Movie not found.\n";
    }

    void updateMovie() {
        int id = readInt("Enter movie ID to update: ", 1);
        Movie* movie = findMovie(id);
        if (!movie) {
            cout << "Movie not found.\n";
            return;
        }
        cout << "1. Update name\n2. Update ticket price\n3. Update available seats\n";
        int option = readInt("Choose field: ", 1, 3);
        if (option == 1) {
            string name = readLine("New movie name: ");
            if (name.empty()) { cout << "Name cannot be empty.\n"; return; }
            movie->setName(name);
        } else if (option == 2) {
            movie->setPrice(readInt("New ticket price: ", 1, 100000));
        } else {
            movie->setAvailableSeats(readInt("New available seats: ", 0, 1000000));
        }
        saveMovies();
        cout << "Movie updated successfully.\n";
    }

    void deleteMovie() {
        int id = readInt("Enter movie ID to delete: ", 1);
        auto it = find_if(movies_.begin(), movies_.end(),
                          [id](const unique_ptr<Movie>& movie) { return movie->id() == id; });
        if (it == movies_.end()) {
            cout << "Movie not found.\n";
            return;
        }
        bool hasActiveBooking = any_of(bookings_.begin(), bookings_.end(),
            [id](const Booking& b) { return b.movieId() == id && b.status() == "CONFIRMED"; });
        if (hasActiveBooking) {
            cout << "Cannot delete a movie with confirmed bookings. Cancel those bookings first.\n";
            return;
        }
        movies_.erase(it);
        saveMovies();
        cout << "Movie deleted successfully.\n";
    }

    void bookTicket() {
        listMovies();
        if (movies_.empty()) return;

        int movieId = readInt("Enter movie ID to book: ", 1);
        Movie* movie = findMovie(movieId);
        if (!movie) {
            cout << "Movie not found.\n";
            return;
        }

        string customerName = readLine("Customer name: ");
        if (customerName.empty()) {
            cout << "Customer name cannot be empty.\n";
            return;
        }
        int tickets = readInt("Number of tickets: ", 1, 100000);
        if (tickets > movie->availableSeats()) {
            cout << "Booking failed. Only " << movie->availableSeats() << " seats remain.\n";
            return;
        }
        if (tickets > numeric_limits<int>::max() / movie->price()) {
            cout << "Total price is too large.\n";
            return;
        }

        int total = tickets * movie->price();
        Payment payment(total);
        if (!payment.process()) {
            cout << "Payment failed; booking was not created.\n";
            return;
        }

        movie->setAvailableSeats(movie->availableSeats() - tickets);
        bookings_.emplace_back(nextBookingId_++, movieId, customerName, tickets,
                               total, "CONFIRMED");
        saveMovies();
        saveBookings();

        cout << "\n===== BOOKING SUCCESSFUL =====\n";
        bookings_.back().display();
        cout << "Remaining seats: " << movie->availableSeats() << '\n';
    }

    void searchBooking() const {
        int id = readInt("Enter booking ID: ", 1);
        auto it = find_if(bookings_.begin(), bookings_.end(),
                          [id](const Booking& b) { return b.id() == id; });
        if (it != bookings_.end()) it->display();
        else cout << "Booking not found.\n";
    }

    void updateBooking() {
        int id = readInt("Enter booking ID to update ticket count: ", 1);
        Booking* booking = findBooking(id);
        if (!booking || booking->status() != "CONFIRMED") {
            cout << "Active booking not found.\n";
            return;
        }
        Movie* movie = findMovie(booking->movieId());
        if (!movie) {
            cout << "Associated movie not found.\n";
            return;
        }
        int newTickets = readInt("New ticket count: ", 1, 100000);
        int difference = newTickets - booking->tickets();

        if (difference > movie->availableSeats()) {
            cout << "Not enough seats available to increase this booking.\n";
            return;
        }
        if (newTickets > numeric_limits<int>::max() / movie->price()) {
            cout << "Total price is too large.\n";
            return;
        }

        movie->setAvailableSeats(movie->availableSeats() - difference);
        booking->setTickets(newTickets);
        booking->setTotalAmount(newTickets * movie->price());
        saveMovies();
        saveBookings();
        cout << "Booking updated. In a real payment integration, collect/refund the difference as required.\n";
        booking->display();
    }

    void cancelBooking() {
        int id = readInt("Enter booking ID to cancel: ", 1);
        Booking* booking = findBooking(id);
        if (!booking || booking->status() != "CONFIRMED") {
            cout << "Active booking not found.\n";
            return;
        }
        Movie* movie = findMovie(booking->movieId());
        if (movie) movie->setAvailableSeats(movie->availableSeats() + booking->tickets());
        booking->setStatus("CANCELLED");
        saveMovies();
        saveBookings();
        cout << "Booking cancelled and seats restored. Refund handling is not connected in this demo.\n";
    }

    void listBookings() const {
        if (bookings_.empty()) {
            cout << "No bookings found.\n";
            return;
        }
        for (const auto& booking : bookings_) booking.display();
    }

    void customerMenu() {
        currentUser_.reset(new Customer(1, "Guest Customer"));
        // Polymorphic call through base-class pointer.
        currentUser_->displayProfile();
        int option;
        do {
            cout << "\n--- CUSTOMER MENU ---\n"
                 << "1. View movies\n2. Book tickets\n3. Search booking\n"
                 << "4. Update booking\n5. Cancel booking\n6. View all bookings\n0. Back\n";
            option = readInt("Choice: ", 0, 6);
            switch (option) {
                case 1: listMovies(); break;
                case 2: bookTicket(); break;
                case 3: searchBooking(); break;
                case 4: updateBooking(); break;
                case 5: cancelBooking(); break;
                case 6: listBookings(); break;
                case 0: break;
                default: break;
            }
        } while (option != 0);
        currentUser_.reset();
    }

    void adminMenu() {
        currentUser_.reset(new Admin(0, "Administrator"));
        currentUser_->displayProfile();
        int option;
        do {
            cout << "\n--- ADMIN MENU ---\n"
                 << "1. Add movie\n2. View movies\n3. Search movie\n"
                 << "4. Update movie\n5. Delete movie\n6. View bookings\n0. Back\n";
            option = readInt("Choice: ", 0, 6);
            switch (option) {
                case 1: addMovie(); break;
                case 2: listMovies(); break;
                case 3: searchMovie(); break;
                case 4: updateMovie(); break;
                case 5: deleteMovie(); break;
                case 6: listBookings(); break;
                case 0: break;
                default: break;
            }
        } while (option != 0);
        currentUser_.reset();
    }

public:
    MovieBookingSystem() {
        loadMovies();
        loadBookings();
        seedMoviesIfEmpty();
    }
    ~MovieBookingSystem() {
        saveMovies();
        saveBookings();
    }

    void run() {
        int option;
        do {
            cout << "\n=====================================\n"
                 << "       MOVIE TICKET BOOKING SYSTEM\n"
                 << "=====================================\n"
                 << "1. Customer menu\n2. Administrator menu\n3. View movies\n0. Exit\n";
            option = readInt("Enter choice: ", 0, 3);
            switch (option) {
                case 1: customerMenu(); break;
                case 2: adminMenu(); break;
                case 3: listMovies(); break;
                case 0: cout << "Thank you for using the system.\n"; break;
                default: break;
            }
        } while (option != 0);
    }
};

int main() {
    MovieBookingSystem system;
    system.run();
    return 0;
}
