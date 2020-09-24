SELECT name
FROM Evolution , Pokemon
WHERE before_id > after_id AND before_id = id
ORDER BY name ASC


