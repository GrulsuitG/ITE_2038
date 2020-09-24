SELECT name
FROM CatchedPokemon, Trainer
WHERE owner_id = Trainer.id AND level <=10
ORDER BY name ASC
