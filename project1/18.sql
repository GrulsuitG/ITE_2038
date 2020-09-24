SELECT AVG(level)
FROM CatchedPokemon, Gym
WHERE owner_id = leader_id 
