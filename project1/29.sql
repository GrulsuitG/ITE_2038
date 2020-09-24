SELECT type, COUNT(*) AS 'NumberOfCatchedPokemon'
FROM CatchedPokemon, Pokemon
WHERE pid = Pokemon.id
GROUP BY type ASC

