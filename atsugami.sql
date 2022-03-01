-- ===================================================================
-- Create tables
-- ===================================================================
CREATE TABLE public.files (
	id SERIAL PRIMARY KEY,
	sha256 VARCHAR(65) UNIQUE NOT NULL,
	rating VARCHAR(1),
	source TEXT,
	count INTEGER,
	created_at TIMESTAMP NOT NULL DEFAULT now(),
	updated_at TIMESTAMP NOT NULL DEFAULT now()
);

CREATE TABLE public.children (
	parent_id INTEGER NOT NULL,
	child_id INTEGER NOT NULL
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
	id SERIAL PRIMARY KEY,
	name TEXT UNIQUE NOT NULL,
	created_at TIMESTAMP NOT NULL DEFAULT now(),
	updated_at TIMESTAMP NOT NULL DEFAULT now()
);

CREATE TABLE public.categories (
	id INTEGER PRIMARY KEY,
	name TEXT UNIQUE NOT NULL
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
	thumb_siz TEXT UNIQUE NOT NULL
);

-- ===================================================================
-- Make the database relational
-- ===================================================================
ALTER TABLE public.children ADD FOREIGN KEY (parent_id) REFERENCES public.files (id) ON DELETE RESTRICT;
ALTER TABLE public.children ADD FOREIGN KEY (child_id) REFERENCES public.files (id) ON DELETE RESTRICT;
ALTER TABLE public.favourites ADD FOREIGN KEY (file_id) REFERENCES public.files (id) ON DELETE RESTRICT;
ALTER TABLE public.files_tags ADD FOREIGN KEY (file_id) REFERENCES public.files (id) ON DELETE CASCADE;
ALTER TABLE public.files_tags ADD FOREIGN KEY (tag_id) REFERENCES public.tags (id) ON DELETE RESTRICT;
ALTER TABLE public.tags_categories ADD FOREIGN KEY (tag_id) REFERENCES public.tags (id) ON DELETE CASCADE;
ALTER TABLE public.tags_categories ADD FOREIGN KEY (category_id) REFERENCES public.categories (id) ON DELETE RESTRICT;
ALTER TABLE public.blacklists ADD FOREIGN KEY (tag_id) REFERENCES public.tags (id) ON DELETE RESTRICT;
ALTER TABLE public.wikis ADD FOREIGN KEY (tag_id) REFERENCES public.tags (id) ON DELETE RESTRICT;

-- ===================================================================
-- Increase the maximum value of sequences by changing the type to
-- 
-- ===================================================================
ALTER SEQUENCE public.files_id_seq AS BIGINT;
ALTER SEQUENCE public.tags_id_seq AS BIGINT;

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
