SELECT Pokemon.name, pid
FROM CatchedPokemon, Trainer, Pokemon
WHERE owner_id = Trainer.id AND 
      pid = Pokemon.id AND 
      hometown = 'Sangnok City'
ORDER BY Pokemon.id ASC
