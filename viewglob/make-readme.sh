#! /bin/sh
# This uses strip-grotty, which I took from the giFTcurs package:
# http://www.nongnu.org/giftcurs/

groff -Tlatin1 -P-cub -ms README.ms | perl ./strip-grotty

