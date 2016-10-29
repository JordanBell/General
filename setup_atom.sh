#!/bin/bash
sudo add-apt-repository ppa:webupd8team/atom
sudo apt-get update
sudo apt-get install atom
apm install nuclide
apm install ocaml-merlin
apm install language-ocaml
