# Graphics-Final-Project
Michael Hanley's ECE560 Computer Graphics Final Project

Requires ANTLR-4 Runtime.

This program can save and load binary "DRW" files containing 2D and 3D graphics. 

Compile with: make
<br>Run with: ./drawing <args>

To load an existing DRW file, run: ./drawing [file name].drw

To convert an SVG file to a DRW file, run: ./drawing [file name].svg
<br>The resulting DRW file will be stored in the drw folder in the main directory, with the same file name as the SVG file.

To run a test case, run: ./drawing <test_number>
<br>Test cases create DRW files from scratch without an SVG file.
<br>Current test cases include:
| test_number | result |
| ---|--- |
| 0 | empty drawing |
| 1 | miscellaneous shapes |
| 2 | wireframes |
| 3 | lit 3D shapes |
