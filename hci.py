import tkinter as tk
from tkinter import messagebox

class BusBookingApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Bus Ticket Booking System")
        self.root.geometry("500x400")
        self.root.resizable(False, False)

        self.user_data = {}
        self.show_home_page()

    # ---------------- HOME PAGE ----------------
    def show_home_page(self):
        self.clear()
        tk.Label(self.root, text="Welcome to Bus Booking System", 
                 font=("Arial", 18, "bold")).pack(pady=30)
        tk.Button(self.root, text="Book a Ticket", bg="#0078D7", fg="white",
                  width=20, command=self.show_search_page).pack(pady=10)
        tk.Button(self.root, text="Exit", bg="#DC3545", fg="white", 
                  width=20, command=self.root.quit).pack(pady=10)

    # ---------------- SEARCH BUS PAGE ----------------
    def show_search_page(self):
        self.clear()
        tk.Label(self.root, text="Search Buses", font=("Arial", 18, "bold")).pack(pady=20)

        tk.Label(self.root, text="From:").pack()
        self.source = tk.Entry(self.root, width=30)
        self.source.pack(pady=5)

        tk.Label(self.root, text="To:").pack()
        self.destination = tk.Entry(self.root, width=30)
        self.destination.pack(pady=5)

        tk.Label(self.root, text="Date of Journey (DD-MM-YYYY):").pack()
        self.date = tk.Entry(self.root, width=30)
        self.date.pack(pady=5)

        tk.Button(self.root, text="Next", bg="#28A745", fg="white",
                  width=15, command=self.show_passenger_page).pack(pady=15)
        tk.Button(self.root, text="Back", bg="#6C757D", fg="white",
                  width=15, command=self.show_home_page).pack()

    # ---------------- PASSENGER DETAILS PAGE ----------------
    def show_passenger_page(self):
        if not self.source.get() or not self.destination.get() or not self.date.get():
            messagebox.showwarning("Missing Details", "Please fill all fields!")
            return
        
        self.user_data['From'] = self.source.get()
        self.user_data['To'] = self.destination.get()
        self.user_data['Date'] = self.date.get()

        self.clear()
        tk.Label(self.root, text="Passenger Details", font=("Arial", 18, "bold")).pack(pady=20)

        tk.Label(self.root, text="Name:").pack()
        self.name = tk.Entry(self.root, width=30)
        self.name.pack(pady=5)

        tk.Label(self.root, text="Age:").pack()
        self.age = tk.Entry(self.root, width=30)
        self.age.pack(pady=5)

        tk.Label(self.root, text="Seat Type:").pack()
        self.seat_type = tk.StringVar(value="Non-AC")
        tk.Radiobutton(self.root, text="AC", variable=self.seat_type, value="AC").pack()
        tk.Radiobutton(self.root, text="Non-AC", variable=self.seat_type, value="Non-AC").pack()

        tk.Button(self.root, text="Next", bg="#28A745", fg="white",
                  width=15, command=self.show_confirmation_page).pack(pady=15)
        tk.Button(self.root, text="Back", bg="#6C757D", fg="white",
                  width=15, command=self.show_search_page).pack()

    # ---------------- CONFIRMATION PAGE ----------------
    def show_confirmation_page(self):
        if not self.name.get() or not self.age.get():
            messagebox.showwarning("Missing Details", "Please enter passenger name and age!")
            return
        
        self.user_data['Name'] = self.name.get()
        self.user_data['Age'] = self.age.get()
        self.user_data['Seat Type'] = self.seat_type.get()

        self.clear()
        tk.Label(self.root, text="Confirm Your Booking", font=("Arial", 18, "bold")).pack(pady=20)

        for key, value in self.user_data.items():
            tk.Label(self.root, text=f"{key}: {value}", font=("Arial", 12)).pack(pady=2)

        tk.Button(self.root, text="Confirm Booking", bg="#0078D7", fg="white",
                  width=20, command=self.show_thankyou_page).pack(pady=15)
        tk.Button(self.root, text="Back", bg="#6C757D", fg="white",
                  width=15, command=self.show_passenger_page).pack()

    # ---------------- THANK YOU PAGE ----------------
    def show_thankyou_page(self):
        self.clear()
        tk.Label(self.root, text="Booking Confirmed!", font=("Arial", 18, "bold")).pack(pady=30)
        tk.Label(self.root, text=f"Thank you {self.user_data['Name']}!", font=("Arial", 12)).pack(pady=5)
        tk.Label(self.root, text="Have a safe journey", font=("Arial", 12)).pack(pady=5)
        tk.Button(self.root, text="Back to Home", bg="#28A745", fg="white",
                  width=20, command=self.show_home_page).pack(pady=15)

    # ---------------- HELPER FUNCTION ----------------
    def clear(self):
        for widget in self.root.winfo_children():
            widget.destroy()


# ---------------- MAIN PROGRAM ----------------
if __name__ == "__main__":
    root = tk.Tk()
    app = BusBookingApp(root)
    root.mainloop()
