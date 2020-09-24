SELECT name, COUNT(*)
FROM CatchedPokemon, Gym, Trainer
WHERE owner_id = leader_id AND Trainer.id = leader_id
GROUP BY owner_id
ORDER BY name ASC
