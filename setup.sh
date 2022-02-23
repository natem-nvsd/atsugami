#!/bin/sh
# Prototype for future use in Atsugami

printf "If PostgreSQL has not already been setup, run this command \`/usr/bin/postgresql-setup --initdb\` as postgres.\n"
ATSUDIR=$HOME/.config/atsugami
THUMBDIR=$ATSUDIR/thumbs
echo "$ATSUDIR"

# Create the required directories
mkdir "$ATSUDIR"
mkdir "$ATSUDIR/files"
mkdir "$THUMBDIR"
mkdir "$THUMBDIR/small"
mkdir "$THUMBDIR/medium"
mkdir "$THUMBDIR/large"
mkdir "$THUMBDIR/huge"
mkdir "$THUMBDIR/gigantic"

# Create an account for the user running the script 
sudo su -c "psql -c 'CREATE USER $USER; ALTER ROLE $USER SUPERUSER;'" postgres
#sudo su -c "psql -c 'ALTER ROLE $USER SUPERUSER;'" postgres
sudo su -c "psql -c 'CREATE DATABASE $USER;'" postgres

# Create an atsugami database for the user running the script
psql -c "CREATE DATABASE atsugami_$USER;"
psql atsugami_$USER < atsugami.sql
psql atsugami_$USER -c "INSERT INTO public.settings (conf_dir, store_dir, thumb_dir, last_dir, thumb_siz) VALUES ('$ATSUDIR', '$ATSUDIR/files', '$ATSUDIR/thumbs', 'file://$HOME', 'medium');"
