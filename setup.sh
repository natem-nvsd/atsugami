#!/bin/sh
# Prototype for future use in Atsugami

ATSUDIR=$HOME/.config/atsugami

# Create the required directories
mkdir $ATSUDIR
mkdir $ATSUDIR/files
mkdir $ATSUDIR/thumbs
mkdir $ATUSDIR/thumbs/small
mkdir $ATUSDIR/thumbs/medium
mkdir $ATUSDIR/thumbs/large
mkdir $ATUSDIR/thumbs/huge
mkdir $ATUSDIR/thumbs/gigantic

# Create an account for the user running the script 
sudo su -c "psql -c 'CREATE USER $USER;'" postgres

# Create an atsugami database for the user running the script
psql -c "CREATE DATABASE atsugami_$USER;"
psql atsugami_$USER < atsugami.sql
psql atsugami -c "INSERT INTO public.settings (store_dir, thumb_dir, thumb_siz) VALUES ('$ATSUDIR/files', '$ATSUDIR/thumbs', 'medium');
