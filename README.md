# PEKS

This is the implementation of public-key searchable encryption using bilinear maps described in 
Public Key Encryption with Keyword Search. It is a simple, command-line application developed in C.

## Implementation Details

#### Dependencies

* GMP Library: 

  ```$ sudo apt-get install libgmp3-dev```
* OpenSSL: 

  ```$ sudo apt-get install libssl-dev```
* [PBC Library](https://crypto.stanford.edu/pbc)

  ```
  $ ./configure --prefix=$HOME/.local
  $ make
  $ make install
  ```
  Makefile uses this path. In case of change in destination directory, update the Makefile accordingly.

#### Build

   ```$ make```

#### Run

   ```$ ./peks <word1> <word2>```

#### Usage

```
$ ./peks hello hello
Equal
$ ./peks Hello hello
Not equal
$ ./peks Supercalifragilisticexpialidocious Supercalifragilisticexpialidocious
Equal
$ ./peks Lopadotemachoselachogaleokranioleipsanodrimhypotrimmatosilphioparaomelitokatakechymenokichlepikossyphophattoperisteralektryonoptekephalliokigklopeleiolagoiosiraiobaphetraganopterygon Lopadotemachoselachogaleokranioleipsanodrimhypotrimmatosilphioparaomelitokatakechymenokichlepikossyphophattoperisteralektryonoptekephalliokigklopeleiolagoiosiraiobaphetraganopterygon
Equal
$ ./peks Lopadotemachoselachogaleokranioleipsanodrimhypotrimmatosilphioparaomelitokatakechymenokichlepikossyphophattoperisteralektryonoptekephalliokigklopeleiolagoiosiraiobaphetraganopterygon Lopadotemachoselachogaleokranioleipsanodrimhypotrimmatosilphioparaomelitokatakechymenokichlepikossyphophattoperisteraletryonoptekephalliokigklopeleiolagoiosiraiobaphetraganopterygon
Not equal
```

*This work was a part of the master thesis from TU Dresden under the supervision of Dr. Josef Spillner and Martin Beck.

## [Public Key Encryption with Keyword Search](http://eprint.iacr.org/2003/195.pdf) 

by Dan Boneh, Giovanni Di Crescenzo, Rafail Ostrovsky and Giuseppe Persiano.

The authors describe a way of appending information to a message that has 
been encrypted with a standard  public key cipher, which allows a server 
that doesn’t have the private key necessary to  decrypt the entire message
to still be able to search for a certain set of keywords. For a keyword, a 
PEKS value can be generated which will allow the server to perform a search
using a trapdoor.
