#!/bin/sh
# Prototype for future use in Atsugami

printf "If PostgreSQL has not already been setup, run this command \`/usr/bin/postgresql-setup --initdb\` as postgres.\n"
CONFIG_ROOT=$HOME/.config/atsugami
STORAGE_ROOT=$HOME/.local/share/atsugami
THUMBDIR=$STORAGE_ROOT/thumbs
echo "$CONFIG_ROOT"
echo "$STORAGE_ROOT"

# Create the required directories
mkdir "$CONFIG_ROOT"
mkdir "$STORAGE_ROOT"
mkdir "$STORAGE_ROOT/files"
mkdir "$THUMBDIR"
mkdir "$THUMBDIR/small"
mkdir "$THUMBDIR/medium"
mkdir "$THUMBDIR/large"
mkdir "$THUMBDIR/huge"
mkdir "$THUMBDIR/gigantic"

# Create an account for the user running the script 
sudo -c "psql -c 'CREATE USER $USER; ALTER ROLE $USER SUPERUSER;'" postgres
sudo -c "psql -c 'CREATE DATABASE $USER;'" postgres

# Create an atsugami database for the user running the script
psql -c "CREATE DATABASE \"${USER}_atsugami\";"
psql "${USER}_atsugami" < atsugami.sql
psql "${USER}_atsugami" -c "INSERT INTO public.settings (conf_dir, store_dir, thumb_dir, last_dir, thumb_size, safe_mode) VALUES ('$CONFIG_ROOT', '$STORAGE_ROOT/files', '$STORAGE_ROOT/thumbs', 'file://$HOME', 1, 'f');"
