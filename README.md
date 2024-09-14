
         ____  ___________      __
       / __ \/ ____/ ____/     / /
      / / / / __/ / __/ __    / / 
     / /_/ / /___/ /___/ /___/ /  
    /_____/_____/_____/\______/  
   #### Text Editor Extraordinaire
   ## Author: ```@Override```

## Will have Vim Motions
    How to use:
        1) if you are in NORMAL mode, then the follow commands work:
            j = <- {direction}
            k = -> {direction}
            h = ^(UP) {direction}
            l = | (DOWN) {direction}
            w = {next word to the RIGHT}
            b = {next word to the LEFT}
            p = PASTE {IMPORTANT: you have to YANK first}
            i = INSERT mode
    
        2) if you are in VISUAL mode:
            {IMPORTANT: you will still be able to use NORMAL MODE motions}
            y = YANK
            i = INSERT mode
            ESC = NORMAL mode
            

        3) if you are in INSERT mode:
            {IMPORTANT: you can type anthing but WILL NOT have NORMAL MODE movements}
            ESC = NORMAL mode 
            

# DEMO:
![ubuntu_OdBKrsX1op](https://github.com/user-attachments/assets/5010c5f5-8090-439d-bef9-386d85e14838)

# Built-in Txt-based scripting language:
    DEEJ is not only just a Note-taking application,
    it is also a text editor and a text-based language with its very own functions.

## **TODO**() Function:

![ubuntu_Z7d0CBZ1A1](https://github.com/user-attachments/assets/4e164541-cc0b-4dfc-8073-961568486f28)

# how to use TODO function:
    Syntax required is as follows:
    
    {it MUST BE WRAPPED WITH DOUBLE ** followed with 'TODO'}
    ie:
    
        **TODO**

    you will also need also need Parenthesis () and content inside of the Parenthesis 
    and ended with a semi-colon ';':
ie: 
-----------------------------------------------------------------------------------------------------------------------------------------
    **TODO**(foo;)
-----------------------------------------------------------------------------------------------------------------------------------------
    this creates a TODO table with the content of `foo`
     |IF YOU WOULD LIKE MULTIPLE ITEMS|
    {IMPORTANT: you MUST have a comma between each item ',':
ie:
----------------------------------------------------------------------------------------------------------------------------------------- 
         **TODO**(foo, bar;)
-----------------------------------------------------------------------------------------------------------------------------------------
    this creates a TODO table with contents `foo` and `bar`.

# Normal mode:

# Insert mode:

# Visual mode:

