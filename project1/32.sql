SELECT name
FROM Pokemon
WHERE id NOT IN(SELECT pid
                FROM Pokemon, CatchedPokemon
                WHERE Pokemon.id =pid)
ORDER BY name ASC
