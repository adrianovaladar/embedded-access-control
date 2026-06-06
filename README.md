# embedded-access-control

<img width="2660" height="1840" alt="image" src="https://github.com/user-attachments/assets/9f803aa9-f39e-48f4-a374-ab77780aa827" />

A keypad based door access control system for the ATmega644 microcontroller. It features PIN authentication with multi-user support, a real-time software clock, an access log and an admin mode for user management.

This project was developed at [ISEP](https://www.isep.ipp.pt).

## Features

- **PIN authentication** — 4-digit codes stored in EEPROM, supporting multiple user IDs;
- **Admin mode** — separate admin account (ID `0000`) with elevated privileges;
- **Door control** — relay/LED output held open for 6 seconds on successful authentication;
- **Real-time software clock** — date and time maintained via Timer0 interrupt, displayed on the LCD between interactions;
- **Access log** — up to 99 timestamped entries (user ID, date, time) browsable by the admin;
- **Date/time setting** — admin can set year, month, day, hour and minute via the keypad;
- **User management** — admin can add users or change passwords for any ID.

## Creators

Project created by [**Adriano Valadar**](https://github.com/adrianovaladar) and [**Jorge Matos**](https://www.linkedin.com/in/jorge-matos-528900139/).
