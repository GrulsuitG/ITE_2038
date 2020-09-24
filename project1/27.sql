SELECT name, MAX(level)
FROM CatchedPokemon, Trainer
WHERE owner_id = Trainer.id
GROUP BY owner_id
HAVING COUNT(*) >=4
ORDER BY name ASC
