CREATE EXTENSION CITEXT;

-- ===================================================================
-- Create tables
-- Add status column once everything is working (in C)
-- The status column can have: 'active', 'deleted', and 'expunged'
--                             'a'       'd'            'e'
-- 'active': The file will be shown in all "normal" icon views.
-- 'deleted': The file will not be shown in "normal" icon views, but
-- the file is still on disk.
-- 'expunged': The file has been deleted from disk, however, records
-- are kept in the database.
-- ===================================================================
-- The "status" column is for if a file has been deleted or not.
-- 0 is for Active		(still visible)
-- 1 is Deleted
-- 2 is Expunged
CREATE TABLE public.files (
	id BIGSERIAL PRIMARY KEY,
	sha256 VARCHAR(65) UNIQUE NOT NULL,
	rating VARCHAR(1),
	source CITEXT,
	created_at TIMESTAMP NOT NULL DEFAULT now(),
	updated_at TIMESTAMP NOT NULL DEFAULT now(),
	status INT NOT NULL DEFAULT 0
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
CREATE TABLE public.favourites (
	file_id INTEGER NOT NULL,
	favourited_at TIMESTAMP NOT NULL DEFAULT now()
);
CREATE TABLE public.files_tags (
	file_id INTEGER NOT NULL,
	tag_id INTEGER NOT NULL
);
CREATE TABLE public.tags (
	id BIGSERIAL PRIMARY KEY,
	name CITEXT UNIQUE NOT NULL,
	created_at TIMESTAMP NOT NULL DEFAULT now(),
	updated_at TIMESTAMP NOT NULL DEFAULT now()
);
CREATE TABLE public.categories (
	id INTEGER PRIMARY KEY,
	name CITEXT UNIQUE NOT NULL
);
CREATE TABLE public.tags_categories (
	tag_id INTEGER NOT NULL,
	category_id INTEGER
);
CREATE TABLE public.blacklists (
	tag_id INTEGER NOT NULL,
	blacklisted_at TIMESTAMP NOT NULL DEFAULT now()
);
CREATE TABLE public.wikis (
	tag_id INTEGER NOT NULL,
	body_text TEXT NOT NULL,
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
	target CITEXT NOT NULL
)

-- ===================================================================
-- Make the database relational
-- ===================================================================
ALTER TABLE public.children ADD FOREIGN KEY (parent_id) REFERENCES public.files (id) ON DELETE RESTRICT;
ALTER TABLE public.children ADD FOREIGN KEY (child_id) REFERENCES public.files (id) ON DELETE RESTRICT;
ALTER TABLE public.favourites ADD FOREIGN KEY (file_id) REFERENCES public.files (id) ON DELETE CASCADE;
ALTER TABLE public.files_tags ADD FOREIGN KEY (file_id) REFERENCES public.files (id) ON DELETE CASCADE;
ALTER TABLE public.files_diffs ADD FOREIGN KEY (file_id) REFERENCES public.files (id) ON DELETE CASCADE;
ALTER TABLE public.files_diffs ADD FOREIGN KEY (file_id) REFERENCES public.diffs (id) ON DELETE CASCADE;
ALTER TABLE public.files_tags ADD FOREIGN KEY (tag_id) REFERENCES public.tags (id) ON DELETE RESTRICT;
ALTER TABLE public.tags_categories ADD FOREIGN KEY (tag_id) REFERENCES public.tags (id) ON DELETE CASCADE;
ALTER TABLE public.tags_categories ADD FOREIGN KEY (category_id) REFERENCES public.categories (id) ON DELETE RESTRICT;
ALTER TABLE public.blacklists ADD FOREIGN KEY (tag_id) REFERENCES public.tags (id) ON DELETE RESTRICT;
ALTER TABLE public.wikis ADD FOREIGN KEY (tag_id) REFERENCES public.tags (id) ON DELETE RESTRICT;
ALTER TABLE public.original_commentary ADD FOREIGN KEY (file_id) REFERENCES public.files (id) ON DELETE CASCADE;
ALTER TABLE public.translated_commentary ADD FOREIGN KEY (file_id) REFERENCES public.files (id) ON DELETE CASCADE;

-- ===================================================================
-- Increase the maximum value of sequences by changing the type to
-- BIGINT (long)
-- ===================================================================
ALTER SEQUENCE public.files_id_seq AS BIGINT;
ALTER SEQUENCE public.tags_id_seq AS BIGINT;
ALTER SEQUENCE public.diffs_id_seq AS BIGINT;

-- ===================================================================
-- Index the database
-- ===================================================================
CREATE INDEX ON public.files ("id", "sha256");
CREATE INDEX ON public.files_tags ("file_id", "tag_id");
CREATE INDEX ON public.children ("parent_id","child_id");
CREATE INDEX ON public.tags ("id", "name");
CREATE INDEX ON public.categories ("id", "name");
CREATE INDEX ON public.tags_categories ("tag_id", "category_id");
CREATE INDEX ON public.wikis ("body_text");

-- ===================================================================
-- Set tag categories. This data should NEVER be changed by the user!
-- Compatitbility with Danbooru should be done in Atsugami;
-- NOT THE DATABASE.
-- ===================================================================
INSERT INTO public.categories (id, name) VALUES (0, 'artist');		-- Danbooru: 1
INSERT INTO public.categories (id, name) VALUES (1, 'copyright');	-- Danbooru: 3
INSERT INTO public.categories (id, name) VALUES (2, 'character');	-- Danbooru: 4
INSERT INTO public.categories (id, name) VALUES (3, 'general');		-- Danbooru: 0
INSERT INTO public.categories (id, name) VALUES (4, 'meta');		-- Danbooru: 5

-- ===================================================================
-- Some meta tags which may come in handy for the user.
--
-- DO NOT ALTER THE id FIELD!!!
-- ===================================================================
INSERT INTO public.tags (id, name) VALUES (1, 'animated');
INSERT INTO public.tags (id, name) VALUES (2, 'animated_gif');
INSERT INTO public.tags (id, name) VALUES (3, 'animated_png');
INSERT INTO public.tags (id, name) VALUES (4, 'artist_request');
INSERT INTO public.tags (id, name) VALUES (5, 'commentary');
INSERT INTO public.tags (id, name) VALUES (6, 'commentary_request');
INSERT INTO public.tags (id, name) VALUES (7, 'copyright_request');
INSERT INTO public.tags (id, name) VALUES (8, 'corrupted_file');
INSERT INTO public.tags (id, name) VALUES (9, 'corrupted_metadata');	-- libexif will be needed for this
INSERT INTO public.tags (id, name) VALUES (10, 'duplicate');
INSERT INTO public.tags (id, name) VALUES (11, 'exif_rotation');	-- libexif will be needed for this
INSERT INTO public.tags (id, name) VALUES (12, 'looping_animation');
INSERT INTO public.tags (id, name) VALUES (13, 'missing_file');		-- Some file magic will be needed to automatically apply this and the next tag.
INSERT INTO public.tags (id, name) VALUES (14, 'missing_thumbnail');
INSERT INTO public.tags (id, name) VALUES (15, 'tagme');
INSERT INTO public.tags (id, name) VALUES (16, 'tall_image');		-- GDK can get the image's resolution.
INSERT INTO public.tags (id, name) VALUES (17, 'ugoria');		-- A Pixiv Ugoria player will be needed.
INSERT INTO public.tags (id, name) VALUES (18, 'ugoria_conversion');
INSERT INTO public.tags (id, name) VALUES (19, 'video');		-- A video player will be needed, and it could also be used to play Ugoria files

INSERT INTO public.tags_categories (tag_id, category_id) VALUES (1, 4);
INSERT INTO public.tags_categories (tag_id, category_id) VALUES (2, 4);
INSERT INTO public.tags_categories (tag_id, category_id) VALUES (3, 4);
INSERT INTO public.tags_categories (tag_id, category_id) VALUES (4, 4);
INSERT INTO public.tags_categories (tag_id, category_id) VALUES (5, 4);
INSERT INTO public.tags_categories (tag_id, category_id) VALUES (6, 4);
INSERT INTO public.tags_categories (tag_id, category_id) VALUES (7, 4);
INSERT INTO public.tags_categories (tag_id, category_id) VALUES (8, 4);
INSERT INTO public.tags_categories (tag_id, category_id) VALUES (9, 4);
INSERT INTO public.tags_categories (tag_id, category_id) VALUES (10, 4);
INSERT INTO public.tags_categories (tag_id, category_id) VALUES (11, 4);
INSERT INTO public.tags_categories (tag_id, category_id) VALUES (12, 4);
INSERT INTO public.tags_categories (tag_id, category_id) VALUES (13, 4);
INSERT INTO public.tags_categories (tag_id, category_id) VALUES (14, 4);
INSERT INTO public.tags_categories (tag_id, category_id) VALUES (15, 4);
INSERT INTO public.tags_categories (tag_id, category_id) VALUES (16, 4);
INSERT INTO public.tags_categories (tag_id, category_id) VALUES (17, 4);
INSERT INTO public.tags_categories (tag_id, category_id) VALUES (18, 4);
INSERT INTO public.tags_categories (tag_id, category_id) VALUES (19, 4);

ALTER SEQUENCE public.tags_id_seq RESTART 20;
