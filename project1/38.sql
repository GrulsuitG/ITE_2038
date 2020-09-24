SELECT name
FROM Evolution, Pokemon
WHERE after_id = id AND
      after_id NOT IN (SELECT before_id
                       FROM Evolution E1)
ORDER BY name ASC
