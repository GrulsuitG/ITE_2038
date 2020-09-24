SELECT COUNT(*)
FROM (SELECT DISTINCT type 
FROM CatchedPokemon, Gym, Pokemon
WHERE owner_id = leader_id AND city = 'Sangnok City'
      AND pid = Pokemon.id) AS NumOfType
