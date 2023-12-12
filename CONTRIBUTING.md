How to contribute
=================


Getting Started
---------------

- Pick an issue from [Issues](https://github.com/cieslarmichal/config-cxx/issues).
- Fork the repository on GitHub

Making Changes
--------------

- Create a feature/bug branch from main branch:

  ``git checkout -b feature/feature-name``

  Please avoid working directly on the ``main`` branch.
- Make commits of logical units.
- Make sure you have added the necessary tests for your changes.
- Run *all* the tests to assure nothing else was accidentally broken.
- If you've added a new file to your project with non-Latin characters, ensure that the file encoding is set to <strong>Unicode (UTF-8 without signature) - Codepage 65001</strong> in Microsoft Visual Studio Code.

Submitting Changes
------------------

- Format code with ``clang-format src/**/*.cpp src/**/*.h include/**/*.h -i -style=file``
- Push your changes to the branch in your fork of the repository.
- Submit a pull request to the repository.

Additional Resources
====================

- If you have any questions about features you can join [Discord Channel](https://discord.com/invite/h2ur8H6mK6).
