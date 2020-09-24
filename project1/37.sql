SELECT name, SUM(level)
FROM Trainer, CatchedPokemon
WHERE owner_id = Trainer.id
GROUP BY owner_id
HAVING SUM(level) >= (SELECT SUM(level)
                      FROM CatchedPokemon
                      GROUP BY owner_id
                      ORDER BY SUM(level) DESC LIMIT 1)
ORDER BY name ASC
