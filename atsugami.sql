CREATE TABLE "files" (
  "id" SERIAL PRIMARY KEY,
  "sha256" VARCHAR(65),
  "created_at" TIMESTAMP NOT NULL DEFAULT now(),
  "tag_string" TEXT NOT NULL DEFAULT ''::text NOT NULL,
  "rating" VARCHAR(1) NOT NULL,
  "parent_id" VARCHAR(65)
);

CREATE TABLE "files_tags" (
  "id" SERIAL PRIMARY KEY,
  "tag_id" INTEGER NOT NULL,
  "file_id" INTEGER UNIQUE NOT NULL
);

CREATE TABLE "tags" (
  "id" SERIAL PRIMARY KEY,
  "name" TEXT NOT NULL,
  "category" INTEGER NOT NULL DEFAULT 0,
  "created_at" TIMESTAMP NOT NULL DEFAULT now()
);

CREATE TABLE "tag_category" (
  "id" SERIAL PRIMARY KEY,
  "name" TEXT UNIQUE NOT NULL
);

CREATE TABLE "wikipages" (
  "id" INTEGER NOT NULL,
  "name" TEXT NOT NULL,
  "urls" TEXT[] NOT NULL,
  "created_at" TIMESTAMP NOT NULL,
  "other_names" TEXT[]
);

CREATE TABLE "settings" (
  "username" TEXT UNIQUE NOT NULL,
  "password" TEXT NOT NULL,	-- sha256 sum
  "storedir" TEXT NOT NULL DEFAULT '/.config/atsugami/files'
);

ALTER TABLE public.files ADD FOREIGN KEY (id) REFERENCES public.files_tags (file_id);
--ALTER TABLE public.files_tags ADD FOREIGN KEY (file_id) REFERENCES public.files (id);

ALTER TABLE public.files ADD FOREIGN KEY (id) REFERENCES public.files (parent_id);
//ALTER TABLE public.files ADD FOREIGN KEY (parent_id) REFERENCES public.files (id);

ALTER TABLE public.files_tags ADD FOREIGN KEY (tag_id) REFERENCES public.tags (id);

ALTER TABLE public.tags ADD FOREIGN KEY (category) REFERENCES public.tag_category (id);

--ALTER TABLE "tags" ADD FOREIGN KEY ("id") REFERENCES "wikipages" ("id");
ALTER TABLE public.wikipages ADD FOREIGN KEY (id) REFERENCES public.tags (id);

CREATE INDEX ON "files" ("id", "parent_id");

CREATE INDEX ON "files" ("sha256", "tag_string");

CREATE INDEX ON "files_tags" ("tag_id", "file_id");

--
-- Set tag categories. Atsugami will never need to set this.
-- Tag categories have these values for compatiblity with Danbooru.
--

INSERT INTO public.tag_category (id, name) VALUES (0, 'general');

INSERT INTO public.tag_category (id, name) VALUES (1, 'artist');

INSERT INTO public.tag_category (id, name) VALUES (3, 'copyright');

INSERT INTO public.tag_category (id, name) VALUES (4, 'character');

INSERT INTO public.tag_category (id, name) VALUES (5, 'meta');
