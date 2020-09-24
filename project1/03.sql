SELECT AVG(level)
FROM Trainer, CatchedPokemon, Pokemon
WHERE Trainer.id = owner_id AND 
      Pokemon.id = pid AND
      Trainer.hometown = 'Sangnok City' AND
      type = 'Electric'