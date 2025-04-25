# Debug Peripheral System (DPS)

![Version](https://img.shields.io/badge/version-3.0.0-blue.svg)
![State](https://img.shields.io/badge/state-stable-green.svg)
![License](https://img.shields.io/badge/license-AGPLv3-blue.svg)

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Repository Structure](#repository-structure)
- [Documentation](#documentation)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)

## Overview

**Debug Peripheral System (DPS)** is a lightweight, portable, and modular C-based system for monitoring and interacting with primitive variables over a CAN network.

DPS follows a master-slave model and is designed for use in embedded environments, with stack-allocated object-oriented interfaces and no dynamic memory usage.

Its goal is to simplify debugging and monitoring of systems across distributed nodes without requiring a debugger connection to each one.

## Features

- 🏦 **Stack-allocated object interface** (no dynamic memory)
- ⚡ **CAN-based communication** between master and slaves
- 🔧 **Variable monitoring** with optional post-update callbacks
- 📋 **Strict encapsulation** and portability across platforms
- 🧻 **Debug-friendly** with variable name lookup and print functions
- 📚 **Doxygen-based documentation**

## Repository Structure

```
DPS/
├── lib/                        # external library
├── src/                        # Core library source
│   ├── master/dps_master.c/h   # Master-side implementation
│   ├── slave/dps_slave.c/h     # Slave-side implementation
│   ├── common/                 # Shared headers and message types
├── tests/                      # Unit and integration tests
├── docs/                       # Auto-generated Doxygen documentation
└── License                     # project License
└── Doxyfile                    # Doxyfile conf file
└── dps_messages.dbc            # Dbc containining the messages used (the ids are not relevant)
└── dps.h                       # Root header file to inclue both the master and slave
```

## Documentation

Full API documentation is generated using Doxygen.

To generate the documentation locally:

```sh
doxygen Doxyfile
```

The output will be in the `docs/` directory.

## Contributing

Contributions are welcome! To contribute:

1. Fork the repository and create a new feature branch:

   ```sh
   git checkout -b my-feature
   ```

2. Make and commit your changes:

   ```sh
   git commit -am "Add feature XYZ"
   ```

3. Push the branch and open a pull request:

   ```sh
   git push origin my-feature
   ```

Please ensure all changes are documented and tested. Stick to the existing coding style and object-based design.

## License

This project is licensed under the GNU AGPLv3 License. See the LICENSE file for details.

## Contact

For issues, suggestions, or questions:

- 📧 Email: [alberto.damo@proton.me](mailto:alberto.damo@proton.me)
- 🐞 GitHub Issues: [Submit an issue](https://github.com/mrmonopoly-cyber/DPS/issues)

---

**DPS** is maintained with care and minimalism in mind — contributions that respect the embedded nature of the project are highly appreciated.
