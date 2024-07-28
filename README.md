# Debug Peripheral System (DPS)

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Repository Structure](#repository-structure)
- [Getting Started](#getting-started)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)

## Overview
The Debug Peripheral System (DPS) is a C-based project focused on facilitating the debugging of peripheral systems in a CAN network.
The project is designed to be modular and extendable, allowing for easy integration and debugging of various peripheral components.
It can work on personal computer as well as in embedded systems.

## Features
- **Modular Design**: Separate modules for master and slave components.
- **Extensive Testing**: Includes a robust testing framework.
- **Easy Integration**: Simplifies the process of integrating new peripherals for debugging.

## Repository Structure
- **lib/**: Contains the core library files.
- **tests/**: Includes test cases for validating the functionality.
- **dps_common.h**: Common header file for shared definitions.
- **dps_master.c / dps_master.h**: Source and header files for the master component.
- **dps_slave.c / dps_slave.h**: Source and header files for the slave component.
- **Makefile**: Build configuration file.

## Getting Started
1. Clone the repository:
   ```sh
   git clone https://github.com/mrmonopoly-cyber/DPS.git
   cd DPS
   ```

1. Build the project:
    ```sh
    make
    ```

## Contributing
Contributions are welcome! Please follow these steps:

1. **Fork the repository**:
   - Go to the GitHub repository and click the "Fork" button.

2. **Create a new branch**:
   - Create a new branch for your feature or bugfix:
     ```sh
     git checkout -b your-feature-branch
     ```

3. **Commit your changes**:
   - Make your changes and commit them with a descriptive message:
     ```sh
     git add .
     git commit -m "Description of your changes"
     ```

4. **Push to your branch**:
   - Push your changes to your forked repository:
     ```sh
     git push origin your-feature-branch
     ```

5. **Create a pull request**:
   - Go to the original repository and click "New Pull Request". Provide a detailed description of your changes and submit the pull request to the master branch.

Please ensure your code adheres to the project's coding standards and passes all tests before submitting a pull request.

## Contact
For any questions, suggestions, or issues, please use one of the following methods:

- **GitHub Issues**: Open an issue on the [GitHub repository](https://github.com/mrmonopoly-cyber/DPS/issues).
- **Email**: Send an email to the project maintainer at [alberto.damo@proton.com](mailto:alberto.damo@proton.me).

We appreciate your feedback and contributions to improve the Debug Peripheral System.
