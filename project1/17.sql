SELECT COUNT(*)
FROM (SELECT DISTINCT pid
      FROM CatchedPokemon, Trainer
      WHERE owner_id = Trainer.id AND hometown = 'Sangnok City') AS CatchedPokemonKind
