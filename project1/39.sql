SELECT name
FROM Trainer
WHERE id IN (SELECT C1.owner_id
             FROM CatchedPokemon C1, CatchedPokemon C2
             WHERE C1.owner_id = C2.owner_id AND
                   C1.pid = C2.pid AND
                   C1.id <> C2.id)
ORDER BY name ASC
