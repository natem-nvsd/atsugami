CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

CREATE TABLE artists (
uuid UUID UNIQUE PRIMARY KEY NOT NULL DEFAULT uuid_generate_v4(),
name TEXT UNIQUE NOT NULL,
created_at TIMESTAMP NOT NULL DEFAULT now(),
updated_at TIMESTAMP NOT NULL DEFAULT now()
);

CREATE TABLE copyrights (
uuid UUID UNIQUE PRIMARY KEY NOT NULL DEFAULT uuid_generate_v4(),
name TEXT UNIQUE NOT NULL,
created_at TIMESTAMP NOT NULL DEFAULT now(),
updated_at TIMESTAMP NOT NULL DEFAULT now()
);

CREATE TABLE characters (
uuid UUID UNIQUE NOT NULL DEFAULT uuid_generate_v4(),
copyright_uuid UUID NOT NULL DEFAULT uuid_generate_v4(),
name TEXT UNIQUE NOT NULL,
created_at TIMESTAMP NOT NULL DEFAULT now(),
updated_at TIMESTAMP NOT NULL DEFAULT now()
);

CREATE TABLE favourites (
uuid REFCURSOR NOT NULL,
favourited_at TIMESTAMP NOT NULL DEFAULT now()
);

CREATE TABLE files (
uuid UUID UNIQUE NOT NULL DEFAULT uuid_generate_v4(),
import_number SERIAL UNIQUE NOT NULL,
path text UNIQUE NOT NULL,
artist TEXT[] NOT NULL DEFAULT '{artist_request}',
copyrights TEXT[] NOT NULL DEFAULT '{copyright_request}',
characters TEXT[],
tags TEXT[] NOT NULL,
source text,
rating VARCHAR(12) NOT NULL,
width INTEGER NOT NULL,
height INTEGER NOT NULL,
is_parent BOOLEAN NOT NULL DEFAULT FALSE,
is_child BOOLEAN NOT NULL DEFAULT FALSE,
has_children BOOLEAN NOT NULL DEFAULT FALSE,
parent_uuid UUID,
child_uuids UUID[],
imported_at TIMESTAMP NOT NULL,
updated_at TIMESTAMP NOT NULL DEFAULT now()
);

CREATE TABLE raw_commentary (
uuid REFCURSOR NOT NULL,
title TEXT UNIQUE NOT NULL,
body TEXT NOT NULL,
created_at TIMESTAMP NOT NULL DEFAULT now(),
updated_at TIMESTAMP NOT NULL DEFAULT now()
);

CREATE TABLE translated_commentary (
uuid REFCURSOR NOT NULL,
title TEXT NOT NULL,
body TEXT NOT NULL,
created_at TIMESTAMP NOT NULL DEFAULT now(),
updated_at TIMESTAMP NOT NULL DEFAULT now()
);

CREATE TABLE tags (
uuid UUID UNIQUE NOT NULL DEFAULT uuid_generate_v4(),
name TEXT UNIQUE NOT NULL,
file_count SERIAL NOT NULL,
created_at TIMESTAMP NOT NULL DEFAULT now(),
updated_at TIMESTAMP NOT NULL DEFAULT now()
);

CREATE TABLE wikipages (
uuid REFCURSOR NOT NULL,
title TEXT UNIQUE NOT NULL,
body TEXT NOT NULL,
created_at TIMESTAMP NOT NULL DEFAULT now(),
updated_at TIMESTAMP NOT NULL DEFAULT now()
);
