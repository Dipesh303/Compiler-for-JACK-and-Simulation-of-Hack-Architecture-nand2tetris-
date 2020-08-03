// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.
(LOOP)
  @KBD
  D=M
  @LOOP
  D;JEQ
  @SCREEN
  D=A
  @0
  M=D
(LOOPB)
  @KBD
  D=A
  @0
  D=M-D
  @ENDB
  D;JGE
  
  @0
  A=M
  M=-1
  @0
  M=M+1
  @LOOPB
  0;JMP

(ENDB)
  @KBD
  D=M
  @ENDB
  D;JNE
   
  @SCREEN
  D=A
  @0
  M=D

(LOOPW)
  @KBD
  D=A
  @0
  D=M-D
  @LOOP
  D;JGE
  
  @0
  A=M
  M=0
  @0
  M=M+1
  @LOOPW
  0;JMP

