# SqlSnake
A class for using mysql in c++

## Experimental Warning
This sql class is experimental and probably has some serious bugs in it. Though it does work according to the provided test program (at least on my machine, lol)<br>
The Following things are supported:<br>
<ul>
<li>Direct Query execution</li>
<li>Database Creation</li>
<li>Table Creation with both primary and foreign keys</li>
<li>Insert statements</li>
<li>Select statements</li>
<li>Delete statements</li>
<li>Update statements
<li>Database deletion</li>
<li>Table deletion</li>
</ul>

## Missing Bits
Currently I'm aware that there are no join statements. For the time being this can be worked around by using the `SqlSnake::newQuery()` function.
