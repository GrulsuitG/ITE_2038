SELECT SUM(level)
FROM CatchedPokemon , Trainer
WHERE owner_id = Trainer.id  AND name = 'Matis'
