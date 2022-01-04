--UPDATE public.files SET path = '~/Pictures/6b6fd343-2271-4c9e-9ac6-9e6665bd99a7.png' WHERE uuid = '6b6fd343-2271-4c9e-9ac6-9e6665bd99a7';
--UPDATE public.files SET PATH = '~/Pictures/uuid.' WHERE uuid = 'UUID';

-- out of date
INSERT INTO public.files (uuid, path, artist, copyrights, characters, tags, source, width, height, is_parent, is_child, parent_uuid, has_children, child_uuids, imported_at) VALUES ('fba16c19-5e67-430c-a2d4-5c49a7c3be24', '/home/natem/Pictures/fba16c19-5e67-430c-a2d4-5c49a7c3be24.jpg', '{karamoneeze}', '{touhou}', '{fujiwara_no_mokou}', '{tagme}', 'https://danbooru.donmai.us/post/show/1156880', 950, 700, 'jpeg', FALSE, FALSE, FALSE, NULL, NULL, now());

INSERT INTO public.copyrights (name) VALUES ('touhou') ON CONFLICT DO NOTHING;
UPDATE public.files SET path = '/home/natem/Pictures/81da0f7f-18c7-48f6-8705-b1f8845b5e51.png' WHERE uuid = '81da0f7f-18c7-48f6-8705-b1f8845b5e51';
SELECT * FROM public.files WHERE artist @> ARRAY['rei_(tonbo0430)'];
SELECT (import_number, uuid, path) FROM public.files ORDER BY import_number ASC;
