#!/usr/bin/env bash

eosio-cpp src/stuff_stake.cpp -o stuff_stake.wasm -abigen -I include -I ./src -I ../shared -R resources -R ricardian