CREATE EXTENSION CITEXT;

-- ===================================================================
-- Create tables
-- ===================================================================



-- ===================================================================
-- Statuses
-- ========
--
-- 0: Active	Default
-- 1: Deleted	Hidden, except in a "Recycle bin" menu
-- 2: Expunged	Erased from disk
-- ===================================================================
CREATE TABLE public.files (
	id BIGSERIAL PRIMARY KEY,
	sha256 VARCHAR(65) UNIQUE NOT NULL,
	rating VARCHAR(1),
	source CITEXT,
	created_at TIMESTAMP NOT NULL DEFAULT now(),
	updated_at TIMESTAMP NOT NULL DEFAULT now(),
	status INT NOT NULL DEFAULT 0,
	favourite BOOLEAN NOT NULL DEFAULT false
);

CREATE TABLE public.children (
	parent_id INTEGER NOT NULL,
	child_id INTEGER NOT NULL
);

CREATE TABLE public.orginal_commentary (
	file_id BIGINT NOT NULL,
	commentary TEXT NOT NULL
);

CREATE TABLE public.translated_commentary (
	file_id BIGINT NOT NULL,
	commentary TEXT NOT NULL
);

CREATE TABLE public.files_tags (
	file_id INTEGER NOT NULL,
	tag_id INTEGER NOT NULL
);

CREATE TABLE public.tags (
	id BIGSERIAL PRIMARY KEY,
	name CITEXT UNIQUE NOT NULL CHECK (name NOT LIKE '%::%'),
	created_at TIMESTAMP NOT NULL DEFAULT now(),
	updated_at TIMESTAMP NOT NULL DEFAULT now(),
	blacklisted BOOLEAN NOT NULL DEFAULT FALSE,
	deprecated BOOLEAN NOT NULL DEFAULT FALSE,
	category SMALLINT NOT NULL DEFAULT 3
);

-- ===================================================================
-- MetaTags
-- ========
--
-- MetaTags are used for special operations involving tags and posts
-- themselves, but not their properties. MetaTags must not be confused
-- with the meta:: category.
--
-- When tagging an image, MetaTags may be used to specify a tag's cat-
-- egory upon creation, instead of the default, `general::`.
-- Example:
-- copyright::touhou
--
-- artist::TEXT
-- character::TEXT
-- copyright::TEXT
-- general::TEXT
-- meta::TEXT
-- post::INT
-- pool::TEXT
-- manga::TEXT
-- rating::INT
-- status::INT
-- 
--
-- Tag categories
-- ==============
--
--   Value |  Atsugami  | Danbooru 
--  -------+------------+----------
--       0 |  Artist    | General  
--       1 |  Copyright | Artist   
--       2 |  Character |
--       3 |  General   | Copyright
--       4 |  Meta      | Character
--       5 |            | Meta     
-- ===================================================================

CREATE TABLE public.wikis (
	tag_id INTEGER NOT NULL,
	body_text TEXT NOT NULL,
	builtin BOOLEAN NOT NULL DEFAULT FALSE,
	created_at TIMESTAMP NOT NULL DEFAULT now(),
	updated_at TIMESTAMP NOT NULL DEFAULT now()
);

CREATE TABLE public.settings (
	conf_dir TEXT UNIQUE NOT NULL,
	store_dir TEXT UNIQUE NOT NULL,
	thumb_dir TEXT UNIQUE NOT NULL,
	last_dir TEXT NOT NULL,
	thumb_size INT UNIQUE NOT NULL DEFAULT 1,
	safe_mode BOOL NOT NULL DEFAULT FALSE
);

CREATE TABLE public.diffs (
	id SERIAL PRIMARY KEY,
	text TEXT NOT NULL,
	edit_date TIMESTAMP NOT NULL DEFAULT now()
);

CREATE TABLE public.files_diffs (
	file_id BIGINT UNIQUE NOT NULL,
	diff_id BIGINT UNIQUE NOT NULL
);

CREATE TABLE public.implications (
	implicator CITEXT NOT NULL,
	implicates CITEXT NOT NULL
)

CREATE TABLE public.aliases (
	alias CITEXT UNIQUE NOT NULL,
	target BIGINT NOT NULL
)

-- ===================================================================
-- The difference between pools and manga is that pools are collections of similar images, while a manga has pages.
-- ===================================================================
CREATE TABLE public.pools (
	id SERIAL PRIMARY KEY,
	name TEXT UNIQUE NOT NULL,
	created_at TIMESTAMP NOT NULL DEFAULT now(),
	updated_at TIMESTAMP NOT NULL DEFAULT now()
);

CREATE TABLE public.pools_files (
	pool_id BIGINT NOT NULL,
	file_id BIGINT NOT NULL
);

CREATE TABLE public.manga (
	id SERIAL PRIMARY KEY,
	artist_id BIGINT UNIQUE NOT NULL,
	name TEXT UNIQUE NOT NULL,
	volume_name TEXT NOT NULL,
	volume_id INT NOT NULL,
	release_year INT,
	created_at TIMESTAMP NOT NULL DEFAULT now(),
	updated_at TIMESTAMP NOT NULL DEFAULT now()
);

CREATE TABLE public.manga_files (
	manga_id BIGINT NOT NULL,
	file_id BIGINT NOT NULL
);

-- ===================================================================
-- Foreign keys
-- ===================================================================
ALTER TABLE public.children ADD FOREIGN KEY (parent_id) REFERENCES public.files (id) ON DELETE RESTRICT;
ALTER TABLE public.children ADD FOREIGN KEY (child_id) REFERENCES public.files (id) ON DELETE RESTRICT;
ALTER TABLE public.files_tags ADD FOREIGN KEY (file_id) REFERENCES public.files (id) ON DELETE CASCADE;
ALTER TABLE public.files_tags ADD FOREIGN KEY (tag_id) REFERENCES public.tags (id) ON DELETE RESTRICT;
ALTER TABLE public.files_diffs ADD FOREIGN KEY (file_id) REFERENCES public.files (id) ON DELETE CASCADE;
ALTER TABLE public.files_diffs ADD FOREIGN KEY (file_id) REFERENCES public.diffs (id) ON DELETE CASCADE;
ALTER TABLE public.wikis ADD FOREIGN KEY (tag_id) REFERENCES public.tags (id) ON DELETE RESTRICT;
ALTER TABLE public.original_commentary ADD FOREIGN KEY (file_id) REFERENCES public.files (id) ON DELETE CASCADE;
ALTER TABLE public.translated_commentary ADD FOREIGN KEY (file_id) REFERENCES public.files (id) ON DELETE CASCADE;
ALTER TABLE public.pools_files ADD FOREIGN KEY (pool_id) REFERENCES public.pools (id) ON DELETE CASCADE;
ALTER TABLE public.pools_files ADD FOREIGN KEY (file_id) REFERENCES public.files (id) ON DELETE CASCADE;
ALTER TABLE public.manga_files ADD FOREIGN KEY (manga_id) REFERENCES public.manga (id) ON DELETE CASCADE;
ALTER TABLE public.manga_files ADD FOREIGN KEY (file_id) REFERENCES public.files (id) ON DELETE CASCADE;

-- ===================================================================
-- Increase the maximum value of sequences by changing the type to
-- BIGINT (long)
-- ===================================================================
ALTER SEQUENCE public.files_id_seq AS BIGINT;
ALTER SEQUENCE public.tags_id_seq AS BIGINT;
ALTER SEQUENCE public.diffs_id_seq AS BIGINT;

-- ===================================================================
-- Indexes
-- ===================================================================
CREATE INDEX ON public.files ("id", "sha256");
CREATE INDEX ON public.files_tags ("file_id", "tag_id");
CREATE INDEX ON public.children ("parent_id","child_id");
CREATE INDEX ON public.tags ("id", "name");
CREATE INDEX ON public.wikis ("body_text");

-- ===================================================================
-- Some metadata tags which may come in handy for the user.
--
-- DO NOT ALTER THE id COLUMN!!!
-- ===================================================================
INSERT INTO public.tags (id, name, category) VALUES (1, 'animated', 4);
INSERT INTO public.tags (id, name, category) VALUES (2, 'animated_gif', 4);
INSERT INTO public.tags (id, name, category) VALUES (3, 'animated_png', 4);
INSERT INTO public.tags (id, name, category) VALUES (4, 'artist_request', 4);
INSERT INTO public.tags (id, name, category) VALUES (5, 'commentary', 4);
INSERT INTO public.tags (id, name, category) VALUES (6, 'commentary_request', 4);
INSERT INTO public.tags (id, name, category) VALUES (7, 'copyright_request', 4);
INSERT INTO public.tags (id, name, category) VALUES (8, 'corrupted_file', 4);
INSERT INTO public.tags (id, name, category) VALUES (9, 'corrupted_metadata', 4);	-- libexif will be needed for this
INSERT INTO public.tags (id, name, category) VALUES (10, 'duplicate', 4);
INSERT INTO public.tags (id, name, category) VALUES (11, 'exif_rotation', 4);	-- libexif will be needed for this
INSERT INTO public.tags (id, name, category) VALUES (12, 'looping_animation', 4);
INSERT INTO public.tags (id, name, category) VALUES (13, 'missing_file', 4);		-- Some file magic will be needed to automatically apply this and the next tag.
INSERT INTO public.tags (id, name, category) VALUES (14, 'missing_thumbnail', 4);
INSERT INTO public.tags (id, name, category) VALUES (15, 'tagme', 4);
INSERT INTO public.tags (id, name, category) VALUES (16, 'tall_image', 4);		-- GDK can get the image's resolution.
INSERT INTO public.tags (id, name, category) VALUES (17, 'ugoria', 4);		-- A Pixiv Ugoria player will be needed.
INSERT INTO public.tags (id, name, category) VALUES (18, 'ugoria_conversion', 4);
INSERT INTO public.tags (id, name, category) VALUES (19, 'video', 4);		-- A video player will be needed, and it could also be used to play Ugoria files

ALTER SEQUENCE public.tags_id_seq RESTART 20;
