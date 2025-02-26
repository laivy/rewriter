USE [game]
GO

DROP PROCEDURE IF EXISTS [dbo].[register_account]
GO

CREATE PROCEDURE [dbo].[register_account]
	@name NVARCHAR(16),
	@password NVARCHAR(16)
AS
BEGIN
	INSERT INTO [dbo].[account]
	VALUES (@name, HASHBYTES('SHA2_256', @password), GETDATE(), NULL)
END
GO
