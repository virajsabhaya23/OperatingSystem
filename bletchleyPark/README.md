# Bletchley-Park-Assignment

### Running the Code
- To run the program with 6 description threads type: <code>./decrypt 6 </code>

- To verify correct decryption type: <code>diff results/ plaintext/ -x .gitignore </code>
    - No output means you correctly decrypted everything.

### Code Structure
- main.c - You have two thread functions you will need to focus on: 
    - receiver_thread() - There is a single receiver thread.  It receives messages from the scheduler and places the messages in the buffer using the insertMessage( ) function. 
    - decryptor_thread() - There are multiple decryption threads.  They read messages from buffer and decrypt them.
    
    <br/>
    
    • The program will crash if the buffer is overfilled. Do not remove this assert. 

    • The program will crash if the buffer is drained beyond 0. Do not remove this assert. 

    • The program will crash if any message waits longer than 3 seconds.  Do not remove this assert. 

- plaintext - This directory holds the plaintext of the encrypted messages. You will use 
these to compare your results. You won’t edit this file. 
- ciphertext - This directory holds the encrypted messages. You won’t edit these messages. 
- schedule.txt - Each line of this file specifies what second a message appears and which encrypted message it is.  You won’t edit this file. 
- clock.c - This file contains the code that keeps the scheduler ticking.  You won’t edit this file. 
- schedule.c - This file reads the schedule.txt data file and serves the messages when they should appear. You won’t edit this file. 