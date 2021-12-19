--UPDATE public.files SET path = '~/Pictures/6b6fd343-2271-4c9e-9ac6-9e6665bd99a7.png' WHERE uuid = '6b6fd343-2271-4c9e-9ac6-9e6665bd99a7';
--UPDATE public.files SET PATH = '~/Pictures/uuid.' WHERE uuid = 'UUID';

INSERT INTO public.files (uuid, path, artist, copyrights, characters, tags, source, width, height, is_parent, is_child, parent_uuid, has_children, child_uuids, imported_at) VALUES ('fba16c19-5e67-430c-a2d4-5c49a7c3be24', '/home/natem/Pictures/fba16c19-5e67-430c-a2d4-5c49a7c3be24.jpg', '{karamoneeze}', '{touhou}', '{fujiwara_no_mokou}', '{tagme}', 'https://danbooru.donmai.us/post/show/1156880', 950, 700, 'jpeg', FALSE, FALSE, FALSE, NULL, NULL, now());
