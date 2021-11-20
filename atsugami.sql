CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

CREATE TABLE artists (
uuid UUID NOT NULL DEFAULT uuid_generate_v4(),
name TEXT NOT NULL,
created_at TIMESTAMP NOT NULL DEFAULT now(),
updated_at TIMESTAMP NOT NULL DEFAULT now()
);

CREATE TABLE copyrights (
uuid UUID NOT NULL DEFAULT uuid_generate_v4(),
name TEXT NOT NULL,
created_at TIMESTAMP NOT NULL DEFAULT now(),
updated_at TIMESTAMP NOT NULL DEFAULT now()
);

CREATE TABLE characters (
uuid UUID NOT NULL DEFAULT uuid_generate_v4(),
copyright_uuid UUID NOT NULL DEFAULT uuid_generate_v4(),
name TEXT NOT NULL,
created_at TIMESTAMP NOT NULL DEFAULT now(),
updated_at TIMESTAMP NOT NULL DEFAULT now()
);

CREATE TABLE favourites (
uuid REFCURSOR NOT NULL,
favourited_at TIMESTAMP NOT NULL DEFAULT now()
);

CREATE TABLE files (
uuid UUID NOT NULL DEFAULT uuid_generate_v4(),
id SERIAL NOT NULL,
path TEXT NOT NULL,
has_children BOOLEAN NOT NULL DEFAULT FALSE,
child BOOLEAN NOT NULL DEFAULT FALSE,
parent_uuid REFCURSOR,
children_uuids REFCURSOR,
artist REFCURSOR NOT NULL,
copyrights REFCURSOR NOT NULL,
characters REFCURSOR NOT NULL,
tags REFCURSOR NOT NULL,
width INTEGER NOT NULL,
height INTEGER NOT NULL,
source TEXT,
format CHARACTER VARYING(4) NOT NULL,
imported_at TIMESTAMP NOT NULL DEFAULT now(),
updated_at TIMESTAMP NOT NULL DEFAULT now()
);

CREATE TABLE raw_commentary (
uuid REFCURSOR NOT NULL,
title TEXT NOT NULL,
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
uuid UUID NOT NULL DEFAULT uuid_generate_v4(),
name TEXT NOT NULL,
file_count SERIAL NOT NULL,
created_at TIMESTAMP NOT NULL DEFAULT now(),
updated_at TIMESTAMP NOT NULL DEFAULT now()
);

CREATE TABLE wikipages (
uuid REFCURSOR NOT NULL,
title TEXT NOT NULL,
body TEXT NOT NULL,
created_at TIMESTAMP NOT NULL DEFAULT now(),
updated_at TIMESTAMP NOT NULL DEFAULT now()
);
