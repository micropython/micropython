#! /usr/bin/env python3

import os
import re
import json

import build_board_info

# Get boards in json format
boards_info_json = build_board_info.get_board_mapping()
# print(boards_info_json)

# TODO (Carlos) Find all the boards on the json format

# We need to know the path of the .travis.yml file
base_path = os.path.dirname(__file__)
travis_path = os.path.abspath(os.path.join(base_path, '..', '.travis.yml'))

# Loading board list based on TRAVIS_BOARDS env variable on .travis.yml
travis_boards = []
with open(travis_path, 'r') as travis:

    # Get all lines that contain the substring 'TRAVIS_BOARDS'
    for line in travis:
        line = travis.readline()

        if 'TRAVIS_BOARDS' in line:
            print('TRAVIS_BOARDS found')
            print(line)
            # TODO (Carlos) Store the line content

        # We've reached the end of the env: section
        elif 'addons' in line:
            break
        else:
            pass

    # TODO (Carlos) Getting all the boards on TRAVIS_BOARDS using regex matching
    # Tranks sommersoft for the pattern
    pattern = '(.+)'

# TODO (Carlos) Comparing boards listed in TRAVIS_BOARDS and boards got from get_board_mapping
