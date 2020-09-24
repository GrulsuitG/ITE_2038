SELECT name, level, nickname
FROM CatchedPokemon , Gym, Pokemon
WHERE owner_id = leader_id  AND 
      pid = Pokemon.id AND
      nickname LIKE 'A%'
ORDER BY name DESC


