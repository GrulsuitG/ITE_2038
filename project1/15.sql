SELECT owner_id, COUNT(*) AS CatchedPokemonNum
FROM CatchedPokemon
GROUP BY owner_id ASC
HAVING COUNT(*) >= (SELECT COUNT(*)
                    FROM CatchedPokemon
                    GROUP BY owner_id
                    ORDER BY COUNT(*) DESC LIMIT 1)