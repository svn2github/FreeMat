#!/bin/sh

# Arg1 is the intel bundle
# Arg2 is the ppc bundle
# Arg3 is the final bundle
echo "Copying Intel bundle to final bundle..."
echo "$1 + $2 --> $3"
cp -R $1 $3
fileset=`cd $1; find . -type f`
for file in $fileset
  do
    lipo -create $1/$file $2/$file -output $3/$file
done
