SELECT hometown, AVG(level)
FROM CatchedPokemon, Trainer
WHERE owner_id = Trainer.id
GROUP BY hometown
ORDER BY AVG(level) ASC

