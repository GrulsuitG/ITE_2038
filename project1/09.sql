SELECT name, AVG(level)
FROM Trainer, CatchedPokemon
WHERE Trainer.id IN (SELECT leader_id FROM Gym)
      AND Trainer.id = owner_id
GROUP BY Trainer.id
ORDER BY name ASC
