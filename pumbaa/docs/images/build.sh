#!/usr/bin/env bash

set -e

# export slides to pdf and then convert them to jpg
sudo soffice --headless --convert-to pdf Pumbaa.odp
convert -density 150 Pumbaa.pdf -quality 95 Pumbaa.jpg

# crop exported slides
convert Pumbaa-0.jpg -crop 860x320+400+460 logo.jpg
convert Pumbaa-1.jpg -crop 1120x900+210+230 architecture.jpg

rm -f Pumbaa.pdf
rm Pumbaa-*.jpg
